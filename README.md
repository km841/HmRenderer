## 2025년 02월 25일</br>
### 00:02</br>
- Texture1D를 이용하여 LUT를 생성 후 연조직과 뼈 구분</br>
- 픽셀 셰이더 내에서 볼륨 텍스쳐에서 샘플링한 밀도값을 LUT를 이용하여 다시 샘플링
![image](https://github.com/user-attachments/assets/6357b1d1-ec58-4347-9e66-e6afe898f694)
- G 키를 입력하여 뼈 부분만 렌더링 / H 키를 입력하여 연조직 부분만 렌더링
![bandicam2025-02-2500-01-00-420-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/6c739257-0b79-4441-ad39-53a2ef469c98)

## 2025년 02월 23일</br>
### 21:08</br>
- 키보드 및 마우스 입력을 담당하는 Input Manager 구현</br>
- 카메라를 독립적인 개체로 월드 내에서 운용할 수 있도록 클래스로 구현하고 월드좌표를 부여</br>
- 월드좌표를 부여함으로써 VP 행렬을 만들 때 자신의 월드좌표를 반영할 수 있도록 하여 결과적으로 카메라의 움직임이 반영되도록 구현했습니다.</br>
- 카메라 이동 (W, S, A, D) 구현</br>
![image](https://github.com/user-attachments/assets/cc08687f-f14c-411d-86e4-6a74f69b8d3c)</br>
- 마우스 우클릭 상태에서 마우스 이동 시 이동량에 따라 회전하도록 구현</br>
- 선형보간 (Lerp) 함수를 구현 및 적용하여 순간 많은 양을 움직이더라도 부드럽게 움직이도록 개선</br>
![bandicam2025-02-2321-03-14-980-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/a765ce94-48b8-44b3-92f9-82708a38e1db)</br>

## 2025년 02월 17일</br>
### 18:16</br>
- Ray-Marching을 이용한 Volume Rendering 구현</br>
- 출처: https://graphicsrunner.blogspot.com/2009/01/volume-rendering-101.html</br>
![skull](https://github.com/user-attachments/assets/ddae2f1c-c4e8-4d8f-b773-130e934a4c14)</br>

## 2025년 02월 15일</br>
### 22:43</br>
- DirectX11 기반 렌더러 Framework 작성 중</br>
![image](https://github.com/user-attachments/assets/9c5eb470-bae7-45f4-8b37-e111a95e43cb)</br>
- 아직 DirectX 관련 코드를 작성하지 않아 Windows API 기반 응용 프로그램 창 표시</br>
- 시간 동기화나 그래픽스 관련된 코드베이스를 작성하기 위한 기반작업 진행</br>
![image](https://github.com/user-attachments/assets/18e64812-21b3-4fd9-a084-f5bf4fec089b)</br>
