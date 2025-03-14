#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstddef>

// Include Schifra headers.
#include "schifra_galois_field.hpp"
#include "schifra_reed_solomon_block.hpp"
#include "schifra_reed_solomon_encoder.hpp"
#include "schifra_reed_solomon_decoder.hpp"
#include "schifra_sequential_root_generator_polynomial_creator.hpp"

int main()
{
    // The file you want to load (size = 1115 bytes for demonstration).
    const std::string input_filename  = "input.bin";
    const std::string output_filename = "encoded_blocks.bin";

    // 1) Read the entire file into memory.
    std::ifstream ifs(input_filename, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "Error: Could not open input file!\n";
        return 1;
    }

    // Read all bytes from file into a buffer.
    std::vector<unsigned char> file_data(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>())
    );
    ifs.close();

    // Check file size (optional sanity check).
    std::size_t file_size = file_data.size();
    std::cout << "Loaded file of size " << file_size << " bytes.\n";


    // 2) Define Reed–Solomon parameters for GF(2^8), code length = 255.
    //    Typically, this means up to 255 symbols per block (1 byte = 1 symbol).
    //    We'll pick a small fec_length (32 parity bytes), leaving 223 data bytes.
    static const std::size_t code_length = 255;   // n
    static const std::size_t fec_length  = 32;    // r
    static const std::size_t data_length = code_length - fec_length; // k = 223


    /* Finite Field Parameters */
    const std::size_t field_descriptor                =   8;
    const std::size_t generator_polynomial_index      = 112;
    const std::size_t generator_polynomial_root_count =  32;

    // Instantiate the Galois Field
    const schifra::galois::field field(field_descriptor,
        schifra::galois::primitive_polynomial_size06,
        schifra::galois::primitive_polynomial06);


    schifra::galois::field_polynomial generator_polynomial(field);
    if (
        !schifra::make_sequential_root_generator_polynomial(
                     field,
                     generator_polynomial_index,
                     generator_polynomial_root_count,
                     generator_polynomial)
      )
   {
      std::cout << "Error - Failed to create sequential root generator!" << std::endl;
      return 1;
   }


    // 4) Create a Reed–Solomon codec descriptor.
    //    This ties together the code length (n), FEC length (r), and data length (k).
    schifra::reed_solomon::encoder<code_length, fec_length> encoder(field, generator_polynomial);

    // We also might instantiate a decoder if we wanted to decode:
    // schifra::reed_solomon::decoder<code_length, fec_length> decoder(field, generator_polynomial_index);

    // 5) Split the input data into 223-byte blocks.
    //    For 1115 bytes, that yields 5 blocks: 4 full blocks of 223, and 1 partial block.
    std::size_t total_blocks = (file_size + data_length - 1) / data_length; // Ceiling division

    // We will store each encoded block (size = code_length bytes) consecutively in output.
    std::ofstream ofs(output_filename, std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Error: Could not open output file for writing!\n";
        return 1;
    }

    // 6) Encode each block.
    for (std::size_t block_index = 0; block_index < total_blocks; ++block_index)
    {
        // Create a container for the data block (k = 223 bytes).
        schifra::reed_solomon::block<code_length, fec_length> block;

        // Figure out the offset and how many bytes we can copy this round.
        std::size_t offset = block_index * data_length;
        std::size_t bytes_to_copy = std::min(data_length, file_size - offset);

        // Initialize the block data with 0 to handle any partial block/padding.
        for (std::size_t i = 0; i < data_length; ++i)
        {
            if (i < bytes_to_copy)
                block.data[i] = file_data[offset + i];
            else
                block.data[i] = 0; // pad with 0 if fewer than data_length remain
        }

        // 7) Perform the Reed–Solomon encoding on this block.
        if (!encoder.encode(block))
        {
            std::cerr << "Error: Encoding failed on block " << block_index << ".\n";
            std::cout << "Error - Critical decoding failure! "
                << "Msg: " << block.error_as_string()  << std::endl;
            return 1;
        }

        // 8) Write out the full codeword (255 bytes: 223 data + 32 parity).
        //    In Schifra’s block struct, the first 'data_length' are data bytes,
        //    the next 'fec_length' are parity bytes.
        for (std::size_t i = 0; i < code_length; ++i)
        {
            ofs.put(static_cast<char>(block.data[i]));
        }
    }

    ofs.close();
    std::cout << "Encoding complete. Wrote " << total_blocks
              << " blocks to " << output_filename << "\n";

    return 0;
}


