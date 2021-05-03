using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace SmartDrawFramework
{
	public class CIni
	{
		/// change to use unicode version - 2011.06.27 modified by humkyung
		[DllImport("kernel32.dll", CharSet=CharSet.Unicode)]
		private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);
		[DllImport("kernel32.dll", CharSet=CharSet.Unicode)]
		private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="section"></param>
		/// <param name="key"></param>
		/// <param name="filePath"></param>
		/// <returns></returns>
		public string GetIniValue(string section, string key, int iSize , string filePath)
		{
			StringBuilder temp = new StringBuilder(iSize);
			int i = GetPrivateProfileString(section, key, "", temp, iSize, filePath);
			return temp.ToString();
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="section"></param>
		/// <param name="key"></param>
		/// <param name="val"></param>
		/// <param name="filePath"></param>
		public void SetIniValue(string section, string key, string val, string filePath)
		{
			WritePrivateProfileString(section, key, val, filePath);
		}

		/// <summary>
		/// INI 파일에 섹션과 키로 검색하여 값을 Integer형으로 읽어 옵니다.
		/// </summary>
		/// <param name="lpAppname">섹션명입니다.</param>
		/// <param name="lpKeyName">키값명입니다.</param>
		/// <param name="nDefalut">기본값입니다.</param>
		/// <param name="lpFileName">파일 이름입니다.</param>
		/// <returns>입력된 값입니다. 만약 해당 키로 검색 실패시 기본 값으로 대체 됩니다.</returns>
		[DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
		public static extern uint GetPrivateProfileInt(string lpAppName, string lpKeyName, int nDefalut, string lpFileName);
	}
}