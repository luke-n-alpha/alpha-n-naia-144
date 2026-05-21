# 생성 계약 리뷰 Round 1 — 2026-05-22

**리뷰어:** Codex (CX), Gemini (GE)
**대상:** contracts/ v0.1 (tech-stack, size-budget, interfaces, tile-spec)

---

## 투표 결과 요약

### CONFIRMED (양쪽 동의)

| 항목 | CX | GE |
|------|:--:|:--:|
| 언어 C99 | O | O |
| Make + gcc | O | O |
| UTF-8 | O | O |
| 더블 버퍼링 | O | O |
| 16색 팔레트 | O | O |
| 글리프 8×16 | O | O |
| 자모 68개 | O | O |
| 비트맵 1,088 bytes | O | O |
| 모듈 분리 구조 | O | O |
| 여유 28% 방향 | O | O |

### DISMISSED (한쪽 이상 반대)

| 항목 | CX | GE | 이유 | 대안 |
|------|:--:|:--:|------|------|
| `-Os -static` | X | - | static이 바이너리 키움 | `-Os -ffunction-sections -fdata-sections -Wl,--gc-sections -s` |
| 렌더 "stdout/raw framebuffer" 혼재 | X | - | 완전 다른 실행환경 | ANSI text 1차 + pixel buffer 별도 백엔드 |
| 폰트 250KB | X | X | 실제는 1KB. 250KB 근거 없음 | 16-32KB 상한 |
| 빌드 오버헤드에 스택/힙 포함 | X | - | 디스크≠RAM | 디스크 예산과 RAM 예산 분리 |
| DialogLine.portrait | X | - | 1.44MB에 초상화 과다 | speaker_id, emotion_id로 축소 |
| Pixel Mode "직접 픽셀 제어" | X | - | 일반 framebuffer와 동일 | 8×16 타일 격자 위의 소형 framebuffer로 재정의 |

### CONTESTED (이견, 대안 있음)

| 항목 | CX | GE | 합의 방향 |
|------|:--:|:--:|----------|
| 플랫폼 Linux x86_64 고정 | 이견 | 동의 | "1차 타깃 Linux x86_64 userland, 실행환경 미정"으로 완화 |
| 오디오 | 1차 무음 권장 | 최소 효과음 유지 | 1차 0KB, stretch 16-32KB 절차음 |
| 압축 zlib/lz4 | miniz 단일파일 또는 비압축 | - | 비압축 우선, 필요시 miniz |
| 디스켓 이미지 | ELF userland 먼저 | FAT12+ELF 모두 | ELF 프로토타입 우선 |
| 밝기 매핑 | 실측 자동화 | 실측 자동화 | on-pixel count 자동 정렬 |
| 시나리오 API 불충분 | reset/get_choices/get_state_id 필요 | - | 보완 |
| 여유 수치 신뢰도 | 재산정 후 20% | 28% 유지 | 재산정 후 20-25% |

### 누락 (양쪽이 지적)

| 누락 | CX | GE |
|------|:--:|:--:|
| 실행 환경 계약 (TTY/console/fb/floppy) | O | - |
| 디스크 예산 vs RAM 예산 분리 | O | O |
| ASCII/기호 글리프 집합 | O | O |
| 입력 키맵 | O | O |
| 저장/세이브 시스템 | O | O |
| 오류 코드 체계 | O | - |
| 프레임 타이밍 목표 | O | - |
| 테스트/측정 계약 | O | - |
| 팔레트 구조체 | O | - |
| dirty region 계약 | O | - |

---

## v0.2 반영 사항

1. 폰트 예산 250KB → 32KB (자모+ASCII+기호)
2. 렌더 분리: ANSI text backend + pixel buffer backend
3. 컴파일 옵션 변경
4. 실행 환경 "ELF userland 프로토타입" 명시
5. 오디오 1차 0KB, stretch goal 절차음
6. 밝기 매핑 → 실측 자동화 방식으로 변경
7. DialogLine.portrait → emotion_id로 축소
8. 시나리오 API 보완 (reset, get_choices, get_state_id)
9. 누락 항목 추가 (키맵, 저장, ASCII, 에러코드, 프레임타이밍)
