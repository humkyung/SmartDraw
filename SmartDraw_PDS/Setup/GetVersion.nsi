!include FileFunc.nsh

OutFile "GetVersion.exe"
SilentInstall silent

Section
    ${GetParameters} $0
    ClearErrors

    ## Get file version
    GetDllVersion "$0" $R0 $R1
    IntOp $R2 $R0 / 0x00010000
    IntOp $R3 $R0 & 0x0000FFFF
    IntOp $R4 $R1 / 0x00010000
    IntOp $R5 $R1 & 0x0000FFFF
    StrCpy $R1 "$R2.$R3.$R4.$R5"

    ## Write it to a !define for use in main script
    FileOpen $R0 "$EXEDIR\VersionNo.nsi" w
    FileWrite $R0 '!define PRODUCT_VERSION "$R1"'
    FileClose $R0

    ## Write version number for jenkins environment variable
    FileOpen $R0 "$EXEDIR\propfile" w
    FileWrite $R0 'BUILD_NAME=$R1'
    FileClose $R0
SectionEnd