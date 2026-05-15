.PHONY: clean all run

vm_main.exe: vm_main.c vm.h config.h
	gcc vm_main.c -o .\bin\vm_main.exe

clean:
	del .\bin\vm_main.exe

run:
	cls
	.\bin\vm_main.exe