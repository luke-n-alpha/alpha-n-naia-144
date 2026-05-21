# Tech Stack — alpha-n-naia-144

**상태:** Draft v0.2 (CX+GE 리뷰 반영)
**변경:** 폰트 예산 정정, 렌더 분리, 컴파일 옵션 변경, 오디오 deferred

---

## 결정

| 항목 | 값 | 근거 |
|------|-----|------|
| **언어** | C (C99) | 1.44MB 예산에서 가장 작은 바이너리 (CX+GE CONFIRMED) |
| **실행 환경** | ELF userland 프로토타입 (패키징 미정) | 공모전 제출 형식 확정 전. Linux x86_64 1차 타깃 (CX CONTESTED→완화) |
| **빌드** | Make + gcc | 최소 빌드 시스템 (CX+GE CONFIRMED) |
| **컴파일 옵션** | `-Os -ffunction-sections -fdata-sections -Wl,--gc-sections -s` | 사이즈 최적화 + dead code 제거. static은 제출 형식 확정 후 검토 (CX DISMISSED 기존 `-Os -static`) |
| **렌더링** | Virtual Framebuffer (ANSI text + pixel buffer 듀얼 백엔드) | 1차: ANSI escape sequence 터미널 렌더. 2차: 소프트웨어 pixel buffer. 공통 CellBuffer 추상화 (CX DISMISSED 기존 혼재) |
| **입력** | stdin (raw mode) 1차 | evdev는 2차 후보 |
| **오디오** | v1: 무음. stretch goal: 절차적 효과음 (16-32KB) | ALSA/OSS는 리스크 대비 효용 낮음 (CX+GE CONTESTED→0KB 기본) |
| **텍스트 인코딩** | UTF-8 | 한글 자모 조합 (CX+GE CONFIRMED) |
| **압축** | 비압축 우선. 필요시 miniz 단일 파일 | zlib/lz4는 빌드 복잡도 과다 (CX CONTESTED) |
| **패키징** | ELF 단일 파일 (프로토타입). FAT12 디스켓은 제출 전 검토 | 제출 형식 미확정 (CX+GE 공통) |
| **색상** | 16색 (4-bit) | 1차 충분 (CX+GE CONFIRMED) |
| **프레임 타이밍** | 턴제 기본, 애니메이션 씬 30fps 목표 | (CX 누락 지적 반영) |

## 대안 검토

| 대안 | 기각 이유 |
|------|----------|
| Zig | 툴체인 안정성, 교차 컴파일 복잡도 |
| Rust | 런타임 오버헤드, 바이너리 사이즈 |
| Assembly | 개발 속도, 유지보수 |
| Python/스크립트 | 사이즈, 성능 |
| static 링크 | 오히려 바이너리 커질 수 있음. 제출 형식 확정 후 재검토 |
