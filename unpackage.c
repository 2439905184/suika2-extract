/* -*- coding: utf-8-with-signature; tab-width: 8; indent-tabs-mode: t; -*- */

/*
 * Suika 2
 * Copyright (C) 2001-2019, TABATA Keiichi. All rights reserved.
 */

/*
 * パッケージ取り出し
 *
 * [Changes]
 *  - 2019/02/03 作成
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ファイルエントリの最大数 */
#define ENTRY_SIZE		(65536)

/* ファイル名のサイズ */
#define FILE_NAME_SIZE		(256)

/* アーカイブ先頭に書き出すファイル数のバイト数 */
#define FILE_COUNT_BYTES	(8)

/* ファイルエントリのサイズ */
#define ENTRY_BYTES		(256 + 8 + 8)

/* パッケージファイル名 */
#define PACKAGE_FILE_NAME	"data01.arc"

/* ファイルエントリ */
struct entry {
	/* ファイル名 */
	char name[FILE_NAME_SIZE];

	/* ファイルサイズ */
	uint64_t size;

	/* ファイルのアーカイブ内でのオフセット */
	uint64_t offset;
} entry[ENTRY_SIZE];

int main(int argc, char *argv[])
{
	FILE *in_fp, *out_fp;
	uint64_t file_count;
	uint64_t i;
	char *p;
	bool success;

	printf("Hello, this is Suika 2's unpackager.\n");

	in_fp = fopen(PACKAGE_FILE_NAME, "rb");
	if (in_fp == NULL) {
		printf("Can't open %s\n", PACKAGE_FILE_NAME);
		return 1;
	}

	if (fread(&file_count, sizeof(uint64_t), 1, in_fp) < 1) {
		printf("Can't read header.\n");
		return 1;
	}

	printf("%lu files found.\n", file_count);

	if (fread(&entry, file_count * sizeof(struct entry), 1, in_fp) < 1) {
		printf("Can't read entries.\n");
		return 1;
	}

	printf("%lu entries read.", file_count);

	for (i = 0; i < file_count; i++) {
		out_fp = fopen(entry[i].name, "wb");
		if (out_fp == NULL) {
			printf("Can't open %s\n", entry[i].name);
			return 1;
		}
		printf("%s\n", entry[i].name);
		fseek(in_fp, entry[i].offset, SEEK_SET);
		p = malloc(entry[i].size);
		if (p == NULL) {
			printf("Memory allocation error.\n");
			return 1;
		}
		if (fread(p, entry[i].size, 1, in_fp) < 1) {
			printf("Cant' read file body.\n");
			return 1;
		}
		printf("Read %lu bytes.\n", entry[i].size);
		if (fwrite(p, entry[i].size, 1, out_fp) < 1) {
			printf("Cant' write file body.\n");
			return 1;
		}
		printf("Wrote %lu bytes.\n", entry[i].size);
		fclose(out_fp);
		free(p);
	}

	printf("Done.\n");

	return 0;
}
