using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace recover
{
    class DrvFile
    {
        private List<DrvItem> m_oDrvItemList = new List<DrvItem>();

        class DrvItem
        {
            private int[] m_iIdentifiers = new int[4];
            private Dictionary<string,string> m_oValueMap = new Dictionary<string,string>();
            private List<string> m_oUnits = new List<string>(){"MM/deg" , "degC" , "MM", "M" , "kg/cm*2 g", "m", "mm"};
            /// <summary>
            /// parse given line
            /// </summary>
            /// <author>humkyung</author>
            /// <date>2014.09.01</date>
            /// <param name="aLine"></param>
            private void ParseValue(string aLine)
            {
                string[] oTokens = aLine.Split(":".ToCharArray());
                if (2 == oTokens.Length)
                {
                    string sLabel = oTokens[0], sValue = oTokens[1];
                    sLabel = sLabel.Trim();
                    sValue = sValue.Trim();
                    if (false == m_oValueMap.ContainsKey(sLabel))
                    {
                        m_oValueMap.Add(sLabel, sValue);
                    }
                    else
                    {
                        m_oValueMap[sLabel] = sValue;
                    }
                }
            }

            /// <summary>
            /// parse drv item value
            /// </summary>
            /// <author>humkyung</author>
            /// <date>2014.09.01</date>
            /// <param name="oLine"></param>
            /// <param name="reader"></param>
            /// <returns></returns>
            private string ParseAttribute(string aLine, ref System.IO.StreamReader reader)
            {
                string aNewLine = aLine;
                if (-1 != aLine.IndexOf("}"))
                {
                    aNewLine = aLine.Substring(aLine.IndexOf("}") + "}".Length);
                    return aNewLine;
                }

                do
                {
                    if(string.IsNullOrEmpty(aNewLine))
                    {
                        aNewLine = reader.ReadLine();
                        aNewLine = aNewLine.Trim();
                    }
                    if(string.IsNullOrEmpty(aNewLine)) continue;
                    if(-1 != aNewLine.IndexOf("}"))
                    {
                        ParseValue(aNewLine.Substring(0 , aNewLine.IndexOf("}")));
                        aNewLine = aNewLine.Substring(aNewLine.IndexOf("}") + "}".Length);
                        return aNewLine;
                    }
                    ParseValue(aNewLine); 
                } while(null != (aNewLine = reader.ReadLine()));

            	throw (new System.Exception("can't find '}' while parsing attribute"));
            }

            /// <summary>
            /// read drv item from file
            /// </summary>
            /// <author>humkyung</author>
            /// <date>2014.09.01</date>
            /// <param name="oLine"></param>
            /// <param name="reader"></param>
            /// <returns></returns>
            public bool Read(string oLine, ref System.IO.StreamReader reader)
            {
                if(-1 == oLine.IndexOf("lbl{")) return false;
                if(-1 != oLine.IndexOf("text {"))
                {
                    string oNewLine = oLine.Substring(oLine.IndexOf("lbl{") + "lbl{".Length , oLine.IndexOf("text {") - oLine.IndexOf("lbl{") - "lbl{".Length);
                    string[] oTokens = oNewLine.Split(" ".ToCharArray() , StringSplitOptions.RemoveEmptyEntries);
                    if(4 == oTokens.Length)
                    {
                        m_iIdentifiers[0] = Convert.ToInt32(oTokens[0]);
                        m_iIdentifiers[1] = Convert.ToInt32(oTokens[1]);
                        m_iIdentifiers[2] = Convert.ToInt32(oTokens[2]);
                        m_iIdentifiers[3] = Convert.ToInt32(oTokens[3]);
                    }

                    oNewLine = oLine.Substring(oLine.IndexOf("text {") + "text {".Length);
                    oNewLine = ParseAttribute(oNewLine , ref reader);

                    if(-1 == oNewLine.IndexOf("}"))
                    {
                        while(null != (oNewLine = reader.ReadLine()))
                        {
                            if(-1 != oNewLine.IndexOf("}")) break;
                        }
                    }
                }

                return true;
            }

            /// <summary>
            /// remove unit string from given string
            /// </summary>
            /// <author>humkyung</author>
            /// <date>2014.09.01</date>
            /// <param name="aLine"></param>
            /// <returns></returns>
            private string RemoveUnit(string aLine)
            {
                foreach (string sUnit in m_oUnits)
                {
                    if (true == aLine.Contains(sUnit))
                    {
                        aLine = aLine.Replace(sUnit , string.Empty);
                    }
                }

                return aLine.Trim();
            }

            /// <summary>
            /// return table no
            /// </summary>
            /// <author>humkyung</author>
            /// <date>2014.09.01</date>
            /// <returns></returns>
            private int GetTableNo()
            {
                int res = 0;

                switch (m_iIdentifiers[1])
                {
                    case 3:
                        res = 34;
                        break;
                    case 5:
                        res = 50;
                        break;
                    case 6:
                        res = 67;
                        break;
                    case 8:
                        res = 80;
                        break;
                }

                return res;
            }

            /// <summary>
            /// return partition no
            /// </summary>
            /// <author>humkyung</author>
            /// <date>2014.09.01</date>
            /// <returns></returns>
            private int GetPartitionNo()
            {
                return (m_iIdentifiers[3] / 8);
            }

            /// <summary>
            /// return system unique no
            /// </summary>
            /// <author>humkyung</author>
            /// <date>2014.09.01</date>
            /// <returns></returns>
            private int GetSystemUniqueNo()
            {
                return (m_iIdentifiers[2] + (GetPartitionNo() * 512 * 1024));
            }

            /// <summary>
            /// return csv string
            /// </summary>
            /// <author>humkyung</author>
            /// <date>2014.09.01</date>
            /// <returns></returns>
            public string ToCSV()
            {
                string res = string.Format("{0},{1},{2}", GetTableNo() , GetSystemUniqueNo() , GetPartitionNo());
                foreach (string sKey in m_oValueMap.Keys)
                {
                    string sValue = m_oValueMap[sKey];
                    sValue = RemoveUnit(sValue);
                    //if (sValue.Contains(' '))
                    //{
                    //    sValue = sValue.Replace(' ', ',');
                    //}

                    res += "," + sValue;
                }

                return res;
            }
        }

        public bool Read(string sFilePath)
        {
            System.IO.StreamReader reader = new System.IO.StreamReader(sFilePath);
		    string aLine;
            while(null != (aLine = reader.ReadLine()))
            {
                aLine = aLine.Trim();
                if(string.IsNullOrEmpty(aLine)) continue;
                if(0 == aLine.IndexOf("pds_file_type")) continue;   /// skip header
                if(0 == aLine.IndexOf("lbl{"))
                {
				    DrvItem oDrvItem = new DrvItem();
				    if(true == oDrvItem.Read(aLine , ref reader))
				    {
                        m_oDrvItemList.Add(oDrvItem);
				    }
				}
			}
            reader.Close();

            return true;
		}

        /// <summary>
        /// write drv items to csv file
        /// </summary>
        /// <author>humkyung</author>
        /// <date>2014.09.01</date>
        /// <param name="sFilePath"></param>
        /// <returns></returns>
        public bool ToCSV(string sFilePath)
        {
            System.IO.StreamWriter writer = new System.IO.StreamWriter(sFilePath);
            foreach (DrvItem oDrvItem in m_oDrvItemList)
            {
                writer.WriteLine(oDrvItem.ToCSV());
            }
            writer.Close();

            return true;
        }
	}
}
