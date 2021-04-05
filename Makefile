CPP=cl

build: so-cpp

so-cpp: main.obj input_processor.obj arg_processor.obj my_string_array.obj hashmap.obj list.obj
	$(CPP) /Zi /Feso-cpp.exe $**
main.obj: main.c     
	$(CPP) /c /MD /nologo /Fe$@ $**
input_processor.obj: input_processor.c
	$(CPP) /c /MD /nologo /Fe$@ $**
arg_processor.obj: arg_processor.c
	$(CPP) /c /MD /nologo /Fe$@ $**
my_string_array.obj: my_string_array.c
	$(CPP) /c /MD /nologo /Fe$@ $**
hashmap.obj: hashmap.c
	$(CPP) /c /MD /nologo /Fe$@ $**
list.obj: list.c
	$(CPP) /c /MD /nologo /Fe$@ $**
clean:
	del *.obj *.exe
