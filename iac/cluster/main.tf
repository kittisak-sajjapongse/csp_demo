terraform {
  required_providers {
    digitalocean = {
      source = "digitalocean/digitalocean"
      version = "~> 2.0"
    }
  }
}

provider "digitalocean" {
  token = var.do_token
}

# Variables
variable "do_token" {
  type = string
}

variable "region" {
  default = "sgp1"
}

resource "digitalocean_project" "vcan_cluster" {
  name        = "vcan_cluster"
  description = "Development infrastructure for EGSE"
  purpose     = "Development of EGSE"
  environment = "Development"
  resources   = concat(
    [for host in digitalocean_droplet.worker : host.urn],
    [digitalocean_droplet.master.urn]
  )
}

# Create a VPC for the private network
resource "digitalocean_vpc" "vcan_cluster_network" {
  name     = "vcan-cluster-network"
  region   = var.region
  ip_range = "10.10.0.0/16"
}

# Array of configurations for nodes representing flatsat devices
variable "hosts" {
  default = [
    {
      name = "flatsat01",
      size = "s-2vcpu-2gb",
    },
    {
      name = "flatsat02",
      size = "s-2vcpu-2gb",
    }
  ]
}

locals {
  cloud_init = <<-EOF
    #cloud-config
    users:
      - name: egse
        gecos: EGSE User
        groups: sudo
        shell: /bin/bash
        sudo: ALL=(ALL:ALL) ALL
        lock_passwd: false
        passwd: $(echo 'egsepoc' | openssl passwd -1 -stdin)
    chpasswd:
      list: |
        root:changeme
      expire: False
    runcmd:
      - sed -i 's/^#ClientAliveInterval.*/ClientAliveInterval 60/' /etc/ssh/sshd_config
      - systemctl restart ssh
      - cd /root; git clone https://github.com/kittisak-sajjapongse/csp_demo.git
      # Sequence matters here in the setup scripts because of 'apt-get install'
      - ./csp_demo/libcsp_setup.sh
      - ./csp_demo/socketcand_setup.sh
      - ./csp_demo/docker_setup.sh
  EOF
}


# Droplet resources for the worker hosts, connecting only to the private network
resource "digitalocean_droplet" "worker" {
  count = length(var.hosts)

  name   = var.hosts[count.index].name
  region = var.region
  size   = var.hosts[count.index].size
  image  = "ubuntu-24-04-x64"

  vpc_uuid = digitalocean_vpc.vcan_cluster_network.id
  ipv6 = false

  # Use cloud-init to change the root password
  user_data = local.cloud_init
}

# master server: public + private network for SSH access
resource "digitalocean_droplet" "master" {
  name   = "master"
  region = var.region
  size   = "s-2vcpu-2gb"
  image  = "ubuntu-24-04-x64"

  # Connect to both public and private networks
  vpc_uuid = digitalocean_vpc.vcan_cluster_network.id
  ipv6 = false
  
  # Enable a public IP for external access
  # ssh_keys = ["your_ssh_key_id"]  # Add your SSH key ID here

  # Use cloud-init to change the root password on master server too
  user_data = local.cloud_init
}

# Create a firewall for internal hosts including worker nodes, allowing SSH only from control plane
resource "digitalocean_firewall" "cluster_firewall" {
  name    = "k8s-cluster-firewall"
  droplet_ids = [for droplet in digitalocean_droplet.worker : droplet.id]

  # Inbound rules: Allow SSH only from the bastion private IP
  inbound_rule {
    protocol         = "tcp"
    port_range       = "22"
    source_addresses = [digitalocean_droplet.master.ipv4_address_private]
  }

  # Allow all outbound traffic from the internal hosts
  outbound_rule {
    protocol         = "tcp"
    port_range       = "all"
    destination_addresses = ["0.0.0.0/0", "::/0"]
  }
}

# Output the private IPs of the three hosts
output "worker_private_ips" {
  value = [for host in digitalocean_droplet.worker : host.ipv4_address_private]
}

# Output the public IP of the master server
output "master_public_ip" {
  value = digitalocean_droplet.master.ipv4_address
}

