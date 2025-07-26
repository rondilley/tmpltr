bin_PROGRAMS = tmpltr
tmpltr_SOURCES = main.c main.h tmpltr.c tmpltr.h parser.c parser.h match.c match.h mem.c mem.h util.c util.h hash.c hash.h bintree.c bintree.h char_class.c ../include/sysdep.h ../include/config.h ../include/common.h
tmpltr_LDADD = 

# Additional security-focused compiler flags
AM_CFLAGS = -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition
AM_CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings
AM_CFLAGS += -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline
AM_CFLAGS += -Wno-long-long -Wundef -Wconversion -Wstrict-overflow=5

# Static Analysis targets
.PHONY: static-analysis cppcheck scan-build splint-check

# Run all static analysis tools
static-analysis: cppcheck scan-build splint-check

# Cppcheck static analysis
cppcheck:
	@echo "Running cppcheck static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c11 --platform=unix64 \
		         --suppress=missingIncludeSystem \
		         --suppress=unusedFunction \
		         --inline-suppr \
		         --xml --xml-version=2 \
		         --output-file=cppcheck-results.xml \
		         $(tmpltr_SOURCES) 2>&1 && \
		echo "Cppcheck results saved to cppcheck-results.xml"; \
	else \
		echo "cppcheck not found. Install with: sudo apt-get install cppcheck"; \
	fi

# Clang Static Analyzer
scan-build:
	@echo "Running Clang Static Analyzer..."
	@if command -v scan-build >/dev/null 2>&1; then \
		$(MAKE) clean && \
		scan-build -o scan-build-results \
		           --html-title="tmpltr Static Analysis" \
		           --keep-cc \
		           -disable-checker deadcode.DeadStores \
		           $(MAKE) && \
		echo "Scan-build results in scan-build-results/"; \
	else \
		echo "scan-build not found. Install with: sudo apt-get install clang-tools"; \
	fi

# Splint static checker
splint-check:
	@echo "Running splint static analysis..."
	@if command -v splint >/dev/null 2>&1; then \
		splint -weak -I../include $(tmpltr_SOURCES) \
		       +posixlib +unixlib \
		       -nullpass -nullret -nullstate \
		       -compdef -usereleased \
		       -temptrans -kepttrans \
		       > splint-results.txt 2>&1 && \
		echo "Splint results saved to splint-results.txt"; \
	else \
		echo "splint not found. Install with: sudo apt-get install splint"; \
	fi

# Clean static analysis results
clean-static:
	@echo "Cleaning static analysis results..."
	@rm -rf scan-build-results/ cppcheck-results.xml splint-results.txt

clean-local: clean-static 
