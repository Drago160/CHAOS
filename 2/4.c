#include<stdio.h>
#include<locale.h>
#include<wchar.h>
#include<stdint.h>
#include<errno.h>
#include<ctype.h>


int main() {
	setlocale(LC_ALL, "");
	wchar_t inp_char;
	int asc_count = 0;
	int utf_count = 0;
	while (((inp_char = getwchar()) != WEOF) && !ferror(stdin)) {
		if (inp_char<=127) {
			asc_count++;

		} else {
			utf_count++;
		}
	}	
	if (ferror(stdin)) {
		printf("%d %d", asc_count, utf_count);
		return 1;
	} 
	printf("%d %d", asc_count, utf_count);
	return 0;
}
