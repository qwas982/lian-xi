long long sdbm(char 串[]){
	long long 哈希 = 0;
	int 甲 = 0;
	while (串[甲] != '\0'){
		哈希 = 串[甲] + (哈希 << 6) + (哈希 << 16) - 哈希;
		甲++;
	}
	return 哈希;
}

long long djb2(char 串[]){
	long long 哈希 = 5381;
	int 甲 = 0;
	while (串[甲] != '\0'){
		哈希 = ((哈希 << 5) + 哈希) + 串[甲];
		甲++;
	}
	return 哈希;
}

char 异或8(char 串[]){
	int 哈希 = 0;
	int 甲 = 0;
	while (串[甲] != '\0'){
		哈希 = (哈希 + 串[甲]) & 0xff;
		甲++;
	}
	return (((哈希 ^ 0xff) + 1) & 0xff);
}

int adler32(char 串[]){
	int 子 = 1;
	int 丑 = 0;
	const int 模态 = 65521;
	int 甲 = 0;
	while (串[甲] != '\0'){
		子 = (子 + 串[甲]) % 模态;
		丑 = (丑 + 子) % 模态;
		甲++;
	}
	return (丑 << 16) | 子;
}

#include <inttypes.h>
uint32_t crc32(char *数据){
	int 甲 = 0;
	uint32_t crc = 0xffffffff;
	while (数据[甲] != '\0'){
		uint8_t 字节 = 数据[甲];
		crc = crc ^ 字节;
		for (int 乙 = 8;乙 > 0;--乙){
			crc = (crc >> 1) ^ (0xEDB88320 & ( -(crc & 1)));

		}
		甲++;
	}
	return crc ^ 0xffffffff;
}