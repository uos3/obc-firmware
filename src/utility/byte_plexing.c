#include <stdint.h>

void data_split_32(int32_t data, uint8_t *split){
    split[0] = ((uint32_t)data & 0x000000ff);
    split[1] = ((uint32_t)data & 0x0000ff00) >> 8;
    split[2] = ((uint32_t)data & 0x00ff0000) >> 16;
    split[3] = ((uint32_t)data & 0xff000000) >> 24;
}

void data_split_u32(uint32_t data, uint8_t *split){
    split[0] = (data & 0x000000ff);
    split[1] = (data & 0x0000ff00) >> 8;
    split[2] = (data & 0x00ff0000) >> 16;
    split[3] = (data & 0xff000000) >> 24;
}

void data_split_u16(uint16_t data, uint8_t *split){
    split[0] = (data & 0x000000ff);
    split[1] = (data & 0x0000ff00) >> 8;
}

void data_split_16(int16_t data, uint8_t *split){
    split[0] = ((uint16_t)data & 0x000000ff);
    split[1] = ((uint16_t)data & 0x0000ff00) >> 8;
}

uint32_t data_combine_u32(uint8_t* split_data){
    // assume each byte little endian, and array little endian
    uint32_t combined_data = 0;
    combined_data += ((uint32_t) split_data[3]);
    combined_data += ((uint32_t) split_data[2] << 8);
    combined_data += ((uint32_t) split_data[1] << 16);
    combined_data += ((uint32_t) split_data[0] << 24);
    return combined_data;
}

uint32_t data_combine_u24(uint8_t *split_data)
{
    // assume each byte little endian, and array little endian
    uint32_t combined_data = 0;
    combined_data += ((uint32_t)split_data[2]);
    combined_data += ((uint32_t)split_data[1] << 8);
    combined_data += ((uint32_t)split_data[0] << 16);
    // combined_data += ((uint32_t)split_data[0] << 24);
    return combined_data;
}
