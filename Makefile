CC=gcc
PROJ=shsubst

$(PROJ): $(PROJ).c
	$(CC) -Wall -Werror -std=c99 -pedantic -o $@ $^

.PHONY: clean install uninstall test
clean:
	rm -f $(PROJ)

install: $(PROJ)
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f $(PROJ) ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/$(PROJ)

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/$(PROJ)

test: $(PROJ)
	@echo Regular replacement
	[ "$$(echo -n '>$${VAR}<' | VAR=TEST ./$(PROJ))" == '>TEST<' ]
	[ "$$(echo -n '>$${VAR}<' | env -i ./$(PROJ))" == '><' ]

	@echo Command execution
	[ "$$(echo -n '>$$(echo -n TEST)<' | ./$(PROJ))" == '>TEST<' ]

	@echo Escaping
	[ "$$(echo -n '$$$${TEST}' | ./$(PROJ))" == '$${TEST}' ]
	[ "$$(echo -n '$$(echo -n \"\(TEST\)\")' | ./$(PROJ))" == '(TEST)' ]
	[ "$$(echo -n 'TE$$T' | ./$(PROJ))" == 'TE$$T' ]

	@echo "OK!"
