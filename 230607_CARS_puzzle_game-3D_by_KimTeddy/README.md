# Description
- Korean
<pre>
3D 그래픽 자동차 주차 퍼즐 게임으로, 논리적 사고와 전략을 사용하여 주어진 문제를 해결하는 게임입니다.
이 게임의 목표는 노란색 자동차를 맨 오른쪽으로 내보내는 것입니다.
각 차량은 배치된 방향으로만 이동할 수 있고, 다른 차량들을 통과할 수 없습니다.
이는 논리적 사고와 전략적 사고를 향상시키는 데 도움이 됩니다.
</pre>
<!--이러한 퍼즐은 난이도에 따라 다양하게 제공되며, 초급부터 전문가 수준의 어려운 퍼즐까지 다양한 레벨이 존재합니다. 또한 일부 게임에서는 제한된 움직임 횟수 내에 목표를 달성하는 등의 도전적인 조건이 추가될 수 있습니다.-->
- English
<pre>
This is a 3D graphic car parking puzzle game where you use logical thinking and strategy to solve given problems.
The goal of the game is to get the yellow car to the far right.
Each vehicle can only move in the direction it is placed and cannot pass other vehicles.
This helps improve logical and strategic thinking.
</pre>
# Functions
### 1. Solution Button
<!--![3D_Car_Escape_Solution](https://github.com/KimTeddy/OpenGL/assets/68770209/a572f343-8544-4169-959e-c41a4dc55bb8)-->
&nbsp;&nbsp;&nbsp;&nbsp; <img src="https://github.com/KimTeddy/OpenGL/assets/68770209/a572f343-8544-4169-959e-c41a4dc55bb8" width="70%">

### 2. Wireframe Mode

&nbsp;&nbsp;&nbsp;&nbsp; <img src="https://github.com/KimTeddy/OpenGL/assets/68770209/6d727a90-6b64-4810-9b97-db094fee77ae" width="70%">
<!--![image](https://github.com/KimTeddy/OpenGL/assets/68770209/6d727a90-6b64-4810-9b97-db094fee77ae)-->

### 3. Change Colors
<!--![image](https://github.com/KimTeddy/OpenGL/assets/68770209/085c03aa-80f5-4e3e-a496-3861d110d817)-->
  - Car Colors
  - Light Colors

  &nbsp;&nbsp;&nbsp;&nbsp; <img src="https://github.com/KimTeddy/OpenGL/assets/68770209/085c03aa-80f5-4e3e-a496-3861d110d817" width="30%">
### 4. Move
<!--![image](https://github.com/KimTeddy/OpenGL/assets/68770209/2db078b9-87ad-40b5-bcf5-30fd512fbd48)-->
  - Translate
    - Pick Car by [list/Radiobutton]
    - Move Cars by Translate Arrow
  - View
    - by Mouse Move
    - by Rotation
    - by translation Z-Axis

  &nbsp;&nbsp;&nbsp;&nbsp; <img src="https://github.com/KimTeddy/OpenGL/assets/68770209/2db078b9-87ad-40b5-bcf5-30fd512fbd48" width="30%">

  
# How it works
1. 초기 세팅 화면
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/717efc3d-0704-43a1-8df5-560ec25aae24)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/432c2113-62cc-4940-8c1f-04acb57dc647)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/7c813477-6b18-4acb-ba80-733a68934490)
</div>
2. 와이어프레임 체크박스 선택한 경우
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/5241e11f-3d7e-4cfa-b747-eb263b9eaf07)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/ae1d1589-8f9a-4e73-a527-2d07dc704b82)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/d64fdefc-7862-45c6-9765-209ab6c7e7c0)
</div>
3. 1번 자동차 선택 후 색상 변경한 모습
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/9a00ca20-a11f-4a1a-acb4-a911d67ee861)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/153f7a5a-44ff-4e3c-98c8-3ddadd676a2e)
</div>
4. 4번 자동차 선택 후 색상 변경한 모습
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/e8644969-9f6d-4dd0-a8c1-fe31bd905038)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/7c1247b9-f37e-4cc6-b4ab-5d1a18049f96)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/b2059b92-43a5-44c9-86ba-a4d180b9893f)
</div>
5. 조명의 색상을 변경한 모습
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/a26cc247-07f1-4483-8567-2fe0d8b65547)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/5a1f491f-653c-451a-b020-ba47aada97bb)
</div>
6. 로테이션을 이용해 원점을 중심으로 판을 회전시킨 모습
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/9ffa7e63-bee3-498e-8e93-4909baa72eeb)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/31f97c51-e864-4181-bedb-3eab1982de62)
</div>
7. Z축 방향으로 멀리 이동시킨 경우
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/5aa6dc02-7ef5-4884-8877-4ea70cc38f33)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/e9e5c947-fb3d-4c7b-af1d-a198d9b36433)
</div>
8. Z축 방향으로 가까이 이동시킨 경우
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/d6539eea-13c3-4277-9032-1bd58126aa9b)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/6e735144-b6b7-46dd-8dce-b031dba8ad04)
</div>
9. 솔루션 애니메이션 시작 버튼 클릭 후 애니메이션 되는 중 모습
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/bd68d337-c246-4c08-a3d4-8f86697deeda)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/bd11b983-d54c-48b8-945a-41bcdf81ff9c)
</div>
10. 게임 완료 후 보이는 “C L E A R” 문구
<div align=center>
  
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/3ac75f33-55dd-45bc-88f9-10745a69ee85)
![image](https://github.com/KimTeddy/OpenGL/assets/68770209/3176229d-a9ea-4022-8a32-5ac97810cafa)
</div>

# 2023 컴퓨터그래픽스 과제2
|게시일|2023.05.23 오후 4:26|
|-|-|
|마감일|2023.06.07 오후 11:59|
|점수공개|2023.06.08 오전 12:00|
|평가점수|100|
|평가의견|모범모델로 사용할 것|
<pre>
과제 : 주차게임(차량 빼내기)을 3D로 구현하고 3D 회전을 이용하여 여러 방향에서 볼 수 있도록 한다.
또한 게임 솔루션을 3D 에니메이션을 통해 해법을 제시할 수 있도록 한다.

제출물 : hwp화일 ( 과제표지, 소스코드, 결과영상 ),  별도 소스코드(hw2.cpp)
과제내용 : 수업시간에 설명합니다.
1. stand alone GLUI 메뉴를 사용하여 GUI를 구현해야 하며, 14가지 control 를 모두 최소 한 번씩은 사용해야 함.
- Static Text : 학번 이름 표시
- Radio Buttons / Listbox : 차량선택에 사용 (Radio button과 Listbox 가 연동되게 해야함.)
- Translation Controls : 차량을 이동 시 사용
- Rotation Controls : 게임판 전체를 회전하여 여러 방향에서 볼 수 있도록 할 때 사용
- Checkboxes : 차량을 wireframe 또는 solid 로 렌더링 선택
- Buttons : 에니메이션 실행 버튼, 종료버튼, 차량 위치 리셋 등에 사용
- EditTextBox / Scrollbar : 차량의 색상 R, G, B 변경시 사용
- Spinners : 조명의 R, G, B 색상변경 시 사용
- Panels, Rollouts, Columns, Separators
2. 애니메이션을 실행하면 초기상태에서 출발하여 최종 완료되는 과정을 자동으로 애니메이션하여 해법을 보여야 함.
3. 3D가 되려면 조명을 사용해야 함. 또한 조명의 R, G, B 값을 변경 가능해야 함.
4. 차량들을 이동하여 최종적으로 노란색 차량이 빠져나갈 수 있게 해야 하며 종료 시 반드시 미션 완료를 표시해야 함.
</pre>
