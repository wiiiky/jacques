/*
 * Copyright (C) 2015 Wiky L
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */
#include "package.h"



/* 解析数据包长度 */
unsigned int decode_package_length(uint8_t *data) {
    int i;
    unsigned int len=0;
    for (i = 0; i < 4; i += 1) {
        len += ((unsigned int)data[i])<<(i*8);
    }
    return len;
}

/* 生成数据包长度的四字节格式 */
void encode_package_length(uint8_t *data, unsigned int len) {
    data[0] = (uint8_t)len;
    data[1] = (uint8_t)(len>>8);
    data[2] = (uint8_t)(len>>16);
    data[3] = (uint8_t)(len>>24);
}
