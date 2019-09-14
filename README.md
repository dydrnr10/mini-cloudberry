# Mini Project
## Objects
소켓을 기반으로 한 공유 디렉토리 시스템\
Linux 의 Shell 과 유사한 형태로 동작하며 간단한 명령어들의 사용이 가능하다.

## Members
황준익 : Makefile, Merge, Server - Client 통신

방진호 : 명령어 기능 구현

신용국 : 명령어 기능 및 옵션 추가

## Build
```bash
$ git clone http://lab.ssafy.com/don-garu/mini-project.git
$ cd mini-project/uploads
$ make
```

## Execute [Server]
```bash
$ cd mini-project/uploads/output
$ ./server <Port>
```

## Execute [Client]
```bash
$ cd mini-project/uploads/output
$ ./client <IP> <Port> [Client]
```

## Clean
```bash
$ cd mini-project/uploads
$ make clean
```

## Command List
- ls [opt] : -l, -i, -a
    ```bash
    ls -opt "dir"
    ```

- cd
    ```bash
    cd "dir"
    ```

- cp
    ```bash
    cp "src" "dest"
    ```

- mkdir
    ```bash
    mkdir "dir"
    ```

- rm
    ```bash
    rm "file"
    ```

- file [opt] : -w, -r, -a
    ```bash
    file -opt "file"
    ```
