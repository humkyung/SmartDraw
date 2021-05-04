# SmartDraw
GA Drawing generation system for PDS

# screenshots
![alt text](https://github.com/humkyung/SmartDraw/blob/main/Docs/login.png?raw=true "new project")
![alt text](https://github.com/humkyung/SmartDraw/blob/main/Docs/new_project.png?raw=true "new project")
![alt text](https://github.com/humkyung/SmartDraw/blob/main/Docs/smartdraw.png?raw=true "new project")

# roadmap
1.1.6
 1. add option that don't generate equipment which has no nozzle
 2. update report

1.1.5
 1. fixed nozzle chart location error
 2. round DIR.,PROJ. value of nozzle chart at first number under dot
 3. apply project unit to nozzle chart and reducer
 4. apply project code for pipe dia. table

1.1.4
 1. #98:detach all reference files from seed file
 2. #99:fixed model file list is not shown
 3.   -:fixed bug related to hsr

1.1.3.32
 1.#73:Match Line Data를 구하는 기능
 2.#74:도면 단위로 Line Id를 구하는 기능
 3.#76:도면에 입력한 Revision Data를 삭제하는 기능
 4.#77:Pipe,Equipment Elevation에 Width Factor가 적용되지 않는 오류 수정
 5.#78:TagEditor를 이용해 Support 이동시 에러 수정
 6:#79:Column No.를 Grid No.에서 구하는 기능
 7.#75:HLR에 Instrument관련 옵션 추가
 8.TagEditor : 90로씩 회전하는 기능 추가
 9.#83:Nozzle Tag 위치 오류 수정 

1.1.2.28
 1. #7:Tag Editor 기능 추가
 2. #55:파이프 태그 영역 설정 기능 추가
 3. #58:Model Offset 기능 추가
 4. #59:External에서 Inside 정렬 오류 수정
 5. #60:Inside Pipe/Eqp Dist 옵션 추가
 6. #61:View Delete 기능 오류 수정
 7. #62:Match line data 저장 오류 수정
 8. #63:Pipe,Eqp에 대해서 모든 뷰의 면에 대해서 정렬 옵션 제공
 9. #64:View Annotation 옵션 추가
 10.#67	HLR에서 Weld Dot가 출력되지 않음
 11.#68	HLR 옵션에 Equipment,Equipment hidden line 설정 기능 추가
 12.#69	View Match Line Data 화면 표기 오류
 13.#70	Dim. Text 크기 자동 조절 옵션 기능
 14.#71	Rev. No 입력 기능 개선	Piping
 15.#72	Rev. Data Manager 기능 개선

1.1.1.27
 1. AutoCAD 도면 생성 기능 추가
 2. HSR의 Line 속성이 적용되지 않는 문제 수정
 3. HLR,Annotation을 각각 실행할수 있는 옵션 추가
 4. BOP 표기 오류 수정
 5. HLR에서 Pipe Centerline 표기 ON/OFF 기능 추가
 6. Support No가 표기되지 않는 오류 수정
 7. HLR 옵션 저장 오류 수정

1.1.0.26
 1. #1:HLR의 각 line을 ON/OFF 하는 기능 추가
 2. #34:KeyPlan 해칭 기능 추가
 3. #35:Dim. Text를 회전하는 기능 추가

1.0.2.25
 1. #27:선택된 도면이 없으면 Generate 버튼을 비활성화 함
 2. #28:Dimension Option Page 추가
 3. #29:리본바에 Setting 버튼 추가
 4. #30:국가별 언어 데이타 파일 수정
 5. #31:Nozzle chart의 설정이 적용되지 않음 수정
 6. #32:출력 파일 열때 프로그램 설치 시 정한 프로그램으로 열기
 7. #33:DRA 파일 세팅 오류

1.0.1.24
 1. #4:Nozzle chart 테이블 생성 오류
 2. #5:Nozzle chart 생성 여부를 설정하는 옵션 추가
 3. #9:1:1 스케일 도면 생성 기능 추가
 4. #18:Oracle Data Dump 개선
 5. #22:VAD 파일을 DRA 파일로 변환하는 기능 제공
 6. #26:생성 도면 이름에 제일 마지막 Rev No를 포함하는 기능

1.0.0.22
 1. Login 방식 변경

1.0.0.21
 1. #30:사용자가 Title Block Data 항목을 추가/삭제 할 수 있도록 수정
 2. #31:Outside View에서 Leader line ON/OFF 오류 수정
 3. #28:External에서 설정한 size보다 큰 pipe만 dimension 표기할 수 있는 옵션 추가

1.0.0.20
 1. FIXED SOME BUGS

1.0.0.17
 1. #25: Title Block Data가 출력되지 않음

1.0.0.13
 1. #23: DWG에서 DRA 파일을 생성하지 못하는 오류 수정

1.0.0.1
 1. Release Beta version
