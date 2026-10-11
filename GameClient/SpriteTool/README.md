# SpriteTool — 스프라이트 시트 편집 툴

Win32와 Direct2D로 만든 스프라이트 시트 편집 도구입니다. 시트 이미지를 불러와 스프라이트 영역을 잘라내고, 그 결과를 파일로 저장합니다. 과외 과제로 시작해 피드백을 받으며 네 차례 다시 설계했고, 이 폴더는 그 마지막 버전입니다.

## 기능

| 조작 | 동작 | 코드 |
|---|---|---|
| 메뉴 `파일 > 불러오기` | 공통 대화상자로 이미지를 골라 WIC로 디코딩 | `ImageView.cpp` `OpenFile` · `GraphicsCore.cpp` `LoadImageFile` |
| 마우스 드래그 | 사각형을 그리면 투명하지 않은 픽셀에 맞춰 영역을 줄여 스프라이트로 등록 | `ImageView.cpp` `OnLButtonUp` · `Image.cpp` `ShrinkRect` |
| 마우스 이동 | 커서 좌표와 그 픽셀의 색을 화면 구석에 표시 | `CoordAndColorView.cpp` |
| 마우스 우클릭 | 클릭한 픽셀의 색을 키 컬러로 지정 | `ImageView.cpp` `OnRButtonDown` |
| 메뉴 `기능 > 배경제거` | 키 컬러와 같은 픽셀의 알파를 0으로 변경 | `Image.cpp` `RemoveKeyColor` |
| 메뉴 `기능 > 자동 슬라이스` | 이어진 픽셀 덩어리를 찾아 스프라이트 영역을 한 번에 추출 | `Image.cpp` `AutoSlice` · `FloodFill` |
| 메뉴 `파일 > 스프라이트 저장 / 불러오기` | 이미지 파일명과 영역 목록을 `.spr` 바이너리로 저장하고 다시 읽음 | `Sprite.cpp` |
| 메뉴 `파일 > png로 저장` | 배경을 지운 이미지를 PNG로 인코딩해 저장 | `GraphicsCore.cpp` `SaveImageFile` |

## 구조

창이 두 개입니다. `CImageView`는 시트 원본과 선택 영역을 보여 주고, `CSpriteView`는 잘라낸 스프라이트를 보여 줍니다.

```
CApp
 ├─ CGraphicsCore   D2D · WIC · DWrite 팩토리 (앱에 하나)
 ├─ CImage          원본 이미지(WIC 비트맵)와 픽셀 연산
 ├─ CImageView  ──┐
 └─ CSpriteView ──┴─ CBaseWindow 상속 · 창마다 렌더 타깃을 따로 가짐
```

- **창과 클래스 연결** — `CBaseWindow`가 `WM_NCCREATE`에서 `this` 포인터를 창에 붙여 두고, 이후 메시지는 가상 함수 `MainProc`으로 넘깁니다 (`BaseWindow.cpp` `SetUpProc`).
- **팩토리는 앱에 하나, 렌더 타깃은 창마다** — 팩토리를 `CGraphicsCore`에 모으고 두 창이 참조로 받아 씁니다. 창은 그래픽 코어를 생성자에서 주입받고, `CBaseWindow`는 Direct2D를 알지 못합니다.
- **원본과 화면용 비트맵 분리** — 원본은 `CImage`가 WIC 비트맵으로 들고 있고, 각 창은 자기 렌더 타깃용 D2D 비트맵을 따로 만듭니다. 배경을 지우거나 이미지를 바꾸면 원본을 고친 뒤 두 창의 화면용 비트맵만 다시 만듭니다 (`ReloadBitmap`).

## 픽셀 연산

- **배경 제거** — 비트맵 전체를 쓰기 잠금한 뒤 줄 포인터를 `stride`만큼, 픽셀 포인터를 4바이트씩 옮기며 비교합니다. 좌표를 매번 곱해 주소를 구하지 않습니다.
- **자동 슬라이스** — 알파가 0이 아닌 픽셀을 만나면 거기서부터 4방향으로 이어진 픽셀을 모두 방문하며 경계 사각형을 넓힙니다. 재귀 대신 스택을 직접 써서, 큰 덩어리에서도 호출 스택이 넘치지 않습니다.

> **빌드 안내** — Win32 데스크톱 애플리케이션입니다. 프로젝트 파일(`.sln`/`.vcxproj`)과 이미지는 저장소에서 제외했으며, 코드 열람 목적입니다. 메뉴 구성은 `SpriteTool.rc`에 있습니다.
