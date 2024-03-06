# 2D 자동차 빼기 퍼즐 게임
- 프로그램 완성 날짜: 2023.05.03
- 이미지 텍스처 맵핑 없이 온전히 코딩으로만 제작한 게임입니다.
- 구조체 수정만으로 쉽게 레벨 변경 가능하도록 설계하였습니다.<br>
![2D_CAR_5MB](https://github.com/KimTeddy/OpenGL/assets/68770209/6a8a45cb-54bf-424d-a58e-d6843d35612a)

# How to PLAY
- 차를 마우스로 드래그하여 최종적으로 노란색 차를 맨 오른쪽으로 나갈 수 있게 만든다.
- Drag the car with the mouse to finally make the yellow car exit to the far right.

# Description
1. 초기 세팅 화면<br>
![1](https://github.com/KimTeddy/OpenGL/assets/68770209/5143809c-4647-46ef-9e8d-5d3e0c298f35)

2. 맨 오른쪽 진한 파란색 트럭을 좌클릭한 상태에서 마우스를 최대한 화면 아래로 움직였을 때<br>
(다른 자동차에 막혀 더 움직일 수 없음, 전조등 켜짐, 백미러 펴짐)<br>
![2](https://github.com/KimTeddy/OpenGL/assets/68770209/610cbe11-9d2d-469a-9b7e-bd885e84a74c)

3. 맨 아래 흰색 자동차를 좌클릭한 상태에서 마우스를 최대한 화면 오른쪽으로 움직였을 때<br>
(화면 밖으로 나가지 못함, 전조등 켜짐, 백미러 펴짐)<br>
![3](https://github.com/KimTeddy/OpenGL/assets/68770209/b66008d5-dfc0-41e2-b4cc-e5dd038b48eb)

4. 3번 상태에서 마우스 버튼을 뗀 후 선택했던 흰색 자동차가 격자와 여백을 두고 자동 정렬된 상태<br>
(전조등 꺼짐, 백미러 접힘)<br>
![4](https://github.com/KimTeddy/OpenGL/assets/68770209/348f1bfe-73c3-4187-9266-db6687997268)

5. 퍼즐을 푼 후 노란색 자동차를 밖으로 이동하기 이전 상태<br>
![5](https://github.com/KimTeddy/OpenGL/assets/68770209/b523f003-eb22-4029-a721-56f8694f3b59)

6. 노란색 자동차를 좌클릭한 상태에서 마우스를 끌어 노란색 자동차를 화면 끝에 놓기 직전 상태<br>
![6](https://github.com/KimTeddy/OpenGL/assets/68770209/c56edc8c-994c-4458-a24e-f81ac899b980)

7. 마우스를 놓으면 자동차가 서서히 나가는 애니메이션이 나오며 “C L E A R” 문자가 뜨고 이는 자동차의 움직임을 따라감<br>
![7](https://github.com/KimTeddy/OpenGL/assets/68770209/37e50a89-b164-4ead-8f78-41201dba7964)

8. “C L E A R” 문자는 끝까지 자동차의 움직임을 따라감<br>
![8](https://github.com/KimTeddy/OpenGL/assets/68770209/028140c4-e625-4448-a867-21aecdfbfa3d)

9. “C L E A R”의 ‘C’가 화면 밖으로 나가기 전까지는 프로그램 유지<br>
![9](https://github.com/KimTeddy/OpenGL/assets/68770209/20c8b2cc-0b3d-43a7-a059-4ed11c7be917)

10. “C L E A R”의 ‘C’까지 화면 밖으로 나가면 프로그램 자동 종료<br>
    ![image](https://github.com/KimTeddy/OpenGL/assets/68770209/42bcdd9f-8f6c-41d5-b13c-ecebb5d7cfed)

# 2023-1학기 컴퓨터그래픽스 과제1
|게시일|2023.04.19 오후 1:31|
|-|-|
|마감일|2023.05.14 오후 11:59|
|점수공개|2023.05.15 오전 12:00|
|제출일시|2023.05.03 오후 10:19:12|
|평가점수|100|
<pre>
과제명 : 차량 빼기 게임

참고사이트 : https://zonzaemgame.com/bbs/board.php?bo_table=Acadegame&wr_id=712
제출물 : hwp화일 ( 소스코드, 결과영상 ),  별도 소스코드(hw1.cpp)
과제내용 : 수업시간에 설명합니다.
1. 위의 링크에 나온 차량 빼내기 프로그램을 작성합니다.
2. 조건은 2차원 평면상에 보인 차량을 선택하고 이동을 할 수 있게 프로그램 되어야 합니다.
3. 차량들을 이동하여 최종적으로 노란색 차량이 빠져나갈 수 있게 해야합니다.
</pre>
