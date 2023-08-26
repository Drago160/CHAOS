#include<stdio.h>
#include<stdint.h>


extern void sum(ITYPE first, ITYPE second, ITYPE* res, int* CF) {
	ITYPE mask = 1;
	int i = sizeof(ITYPE) * 8;
	ITYPE mask1;
	ITYPE mask2;
	ITYPE cf_mask = 0;
	*res = 0;
	while (i > 0) {
		mask1 = (first&mask);
		mask2 = (second&mask);
		*res = *res | ((mask1^mask2)^cf_mask);//a+b+c = (a^b)^c
		cf_mask = (mask1&mask2)|(mask1&cf_mask)|(mask2&cf_mask);//maj with 3 elem(СДНФ)
		mask<<=1;
		cf_mask<<=1;
		i--;
	}
	*CF = (cf_mask!=0)?1:0;
}
