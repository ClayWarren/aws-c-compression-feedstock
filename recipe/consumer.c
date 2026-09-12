#include <aws/compression/huffman.h>
#include <stdio.h>
#include <string.h>
static struct aws_huffman_code encode(uint8_t symbol, void *ctx) {
    (void)ctx;
    struct aws_huffman_code code = {symbol == 'B' ? 1 : 0, 1};
    if (symbol != 'A' && symbol != 'B') code.num_bits = 0;
    return code;
}
static uint8_t decode(uint32_t bits, uint8_t *symbol, void *ctx) {
    (void)ctx;
    *symbol = (bits >> 31) ? 'B' : 'A';
    return 1;
}
int main(void) {
    struct aws_huffman_symbol_coder coder = {encode, decode, NULL};
    struct aws_huffman_encoder encoder;
    struct aws_huffman_decoder decoder;
    uint8_t encoded[8], decoded[16];
    struct aws_byte_buf output = aws_byte_buf_from_empty_array(encoded, sizeof(encoded));
    struct aws_byte_buf restored = aws_byte_buf_from_empty_array(decoded, sizeof(decoded));
    struct aws_byte_cursor input = aws_byte_cursor_from_c_str("ABABBAAB");
    aws_huffman_encoder_init(&encoder, &coder);
    aws_huffman_decoder_init(&decoder, &coder);
    if (aws_huffman_get_encoded_length(&encoder, input) != 1) return 1;
    if (aws_huffman_encode(&encoder, &input, &output) || input.len || output.len != 1 || encoded[0] != 0x59) return 2;
    input = aws_byte_cursor_from_buf(&output);
    if (aws_huffman_decode(&decoder, &input, &restored) || restored.len != 8 || memcmp(decoded, "ABABBAAB", 8)) return 3;
    puts("Installed Huffman known-answer round trip passed");
    return 0;
}
