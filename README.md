# Multithreading cp -R

## Compile (Oracle Solaris 10)
```
cc -m64 -mt -lpthread path/path.c list/list.c file/multi_close.c file/multi_open.c file/cp_dir.c file/cp_file.c file/cp_r.c main.c -o main.a
```

## run
```
./main.a {source_path} {dest_path}
```
