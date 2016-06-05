eco: eco.o ecoCalculations.o ecoArrayAllocate.o ecoPlots.o ecoMisc.o eco.h
	gcc -o eco eco.o ecoCalculations.o ecoArrayAllocate.o ecoPlots.o ecoMisc.o -lpgplot -lcpgplot -lX11 -lm

eco.o: eco.c eco.h
	gcc -c eco.c

ecoCalculations.o: ecoCalculations.c eco.h
	gcc -c ecoCalculations.c

ecoArrayAllocate.o: ecoArrayAllocate.c eco.h
	gcc -c ecoArrayAllocate.c

ecoPlots.o: ecoPlots.c eco.h
	gcc -c ecoPlots.c
	
ecoMisc.o: ecoMisc.c eco.h
	gcc -c ecoMisc.c
