#include<stdio.h>
#include<stdint.h>


uint64_t makeMask(int n) {
	uint64_t one = 1;
	return one<<(n-1);
}


uint64_t makeSet(char c) {
	if (('0' <= c) && (c <= '9')) {
		return makeMask(c-'0'+1);
	} else if (('a' <= c) && (c <= 'z')) {
		return makeMask((c-'a')+11);
	} else if (('A' <= c) && (c <= 'Z')) {
		return makeMask(c - 'A' + 'z'-'a' + 2 + 11);
	}
	return 0;
}


void unSet(uint64_t set) {
	uint64_t pivot = 1;
	for (int i = 0; i < 10; i++) {
		if ((pivot&set) != 0) {
			printf("%c", '0'+i);
		}
		pivot <<= 1;
	}
	pivot <<= 27;
	for (int i = 0; i < 26; i++) {
		if ((pivot&set) != 0) {
			printf("%c", 'A'+i);
		}
		pivot <<= 1;
	}
	pivot >>= 53;
	for (int i = 0; i < 26; i++) {
		if ((pivot&set) != 0) {
			printf("%c", 'a'+i);
		}
		pivot <<= 1;
	}
	




}


int main() {
	
	char c;	
	
	uint64_t main_set = 0;
	uint64_t tmp_set = 0;

	while ((c = getchar()) != EOF) {
		if (c == '\n') {
			continue;
		}
		if (c == '&') {
			main_set = tmp_set & main_set;	
			tmp_set = 0;
		} else if (c == '|') {
			main_set = tmp_set | main_set;
			tmp_set = 0;
		} else if (c == '^') {
			main_set = tmp_set ^ main_set;	
			tmp_set = 0;
		} else if (c == '~') {
			if (tmp_set == 0) {
				main_set = ~main_set;
			} else {
				tmp_set = ~tmp_set;	
			}
		} else {
			tmp_set = tmp_set | makeSet(c);
		}
	}
	unSet(main_set);
	return 0;
}
