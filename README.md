# SystemProgramming
CSE4100 System Programming Projects 2019

## Project 1 : SIC/XE shell 
* object code가 적재되고 실행될 메모리 공간, opcode table 관련 명령어 구현
* mnemonic을 opcode로 변환, opcode는 Hash Table로 구현, hash 충돌 시 linked list의 형태로 구현

## Project 2 : Assembler
* asm파일 입력 받아 에러 체크 및 obj 파일 생성
* obj 파일과 symbol table을 볼 수 있는 기능을 제공

## Project 3 : Linking, Loading
* assemble로 생성된 obj파일을 link시켜 메모리에 로드
* breakpoint를 설정하여 debugging, 프로그램을 Run할 수 있는 기능 구현

## Project 4 : python 연습

## Project 5 : Machine Learing
* CIFAR-10 데이터 셋을 활용하여 이미지를 10개의 클래스로 분류
* 샘플 코드를 수정하여 classify 정확도 75%+
* epoch, batch size, learning rate, Convolution layer 추가, Drop out layer 추가

## Project 6 : MapReduce
* Azure환경에서 Hadoop의 분산 처리 기법인 Map&Reduce를 구현
* 대용량의 데이터에 대해 Group by Max 알고리즘을 Mapping, Reducing으로 구현
