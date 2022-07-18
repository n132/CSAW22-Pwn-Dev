target = chal1 chal2 chal3 chal4 chal5

all: $(target)

chal1: chal/1st/chal1.c
	gcc -z execstack $^ -o $@

chal2: chal/2nd/chal2.c
	gcc -z execstack $^ -o $@

chal3: chal/3rd/chal3.c
	gcc -z execstack $^ -o $@

chal4: chal/4th/chal4.c
	gcc -z execstack $^ -o $@

chal5: chal/5th/chal5.c
	gcc $^ -o $@

clean:
	rm $(target)