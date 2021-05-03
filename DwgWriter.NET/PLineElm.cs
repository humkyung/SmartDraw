using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.Geometry;
using Autodesk.AutoCAD.ApplicationServices;

namespace aDwgWriter
{
    class DimensionElm : WriteElm
    {
        private Point3d m_ptFirst, m_ptSecond, m_ptLoc;
        private double m_dDimScale;
        private string m_sDimDirec, m_sTextDirec;
        private string m_sDimStyle;
       
        public override bool Parse(string aLine, StreamReader oSR)
        {
            string[] tokens = aLine.Split("|".ToCharArray());
            if (3 == tokens.Length)
            {
                string[] subtokens = tokens[1].Split(",".ToCharArray());
                if (7 == subtokens.Length)
                {
                  
                    m_ptFirst = new Point3d(Convert.ToDouble(subtokens[0]), Convert.ToDouble(subtokens[1]), 0.0);
                    m_ptSecond = new Point3d(Convert.ToDouble(subtokens[2]), Convert.ToDouble(subtokens[3]), 0.0);
                    m_ptLoc = new Point3d(Convert.ToDouble(subtokens[4]), Convert.ToDouble(subtokens[5]), 0.0);
                    m_dDimScale = Convert.ToDouble(subtokens[6]);
                }
                else
                {
                    return false;
                }

                subtokens = tokens[2].Split(",".ToCharArray());
                if (4 == subtokens.Length)
                {
                    mLayer = subtokens[0];
                    m_sDimDirec = subtokens[1];
                    m_sTextDirec = subtokens[2];
                    m_sDimStyle = subtokens[3];                    
                }
                else
                {
                    return false;
                }

                return true;
            }

            return false; 
        }

        public override Entity GetEntity()
        {
            ObjectId objid = new ObjectId();         
            
            AlignedDimension oDimension = new AlignedDimension(m_ptFirst, m_ptSecond, m_ptLoc, string.Empty, objid);
            {
                oDimension.Dimlfac = m_dDimScale;
                oDimension.Dimtix = true;
                if(m_sTextDirec == "D")
                    oDimension.Dimtad =4; 
                oDimension.Layer = mLayer;                
            }
            return oDimension;
           
        }
     
    }
}
