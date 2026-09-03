/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 
//#include "SQLDDL.h" 
/******后台pc文件标准注释标记*****/
/* =========================================================================
/// <remark>
/// <summary>
/// 炼钢钢种变更函数
/// <para>  </para>
/// <para>  </para>
/// </summary>
/// <param name= "xxxx">输入参数</param>
/// <returns>成功:0</returns>
/// <returns>失败:-1</returns>
/// </remark>
========================================================================= */
// 函数入口
 
BM2_FUNCTION_EXPORT
 int f_mmsm0055_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int atFlag = 0;
	int i;
	int blkNum;
	int fetchRowCount;
	CString datetime = "";
 	CString c_datetime = "";
	int  i_count = 0;	
	CString c_heat_confm_flag  ="0";
	CString c_pono_in = "";       //原PONO号
	CString c_st_no_in = "";     //原出钢记号
	CString c_pono_out = "";      //变更PONO号
	CString c_st_no_out = "";    //变更出钢记号
	CString recReviseTime = "";
	CString recRevisor = "";
	CString sqlstr = "";
	int sqlid = 0;
	CString updPono = "";
	CString updStNo = "";

	CDbCommand updateCmd(conn);
 

	try
	{
		if(bcls_rec->Tables.Contains("MMSM0055") == false)
		{
			 strcpy(s.msg,_RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			 strcpy(s.sysmsg, "块MMSMSM不存在。");
			 throw CApplicationException(-1, s.msg, log.Location); 
		}

		c_pono_in = bcls_rec->Tables["MMSM0055"].Rows[0]["PONO_IN"].ToString().Trim();
		c_st_no_in = bcls_rec->Tables["MMSM0055"].Rows[0]["ST_NO_IN"].ToString().Trim();
		c_pono_out = bcls_rec->Tables["MMSM0055"].Rows[0]["PONO_OUT"].ToString().Trim();
		c_st_no_out = bcls_rec->Tables["MMSM0055"].Rows[0]["ST_NO_OUT"].ToString().Trim();

		if(c_pono_in == "")
		{
			sprintf(s.sysmsg, "原制造命令号不能为空"); //系统错误信息
			strcpy(s.msg, _RES("MMSMS0000106")/*"原制造命令号不能为空!"*/); //用户提示信息，国际化
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		if(c_st_no_in == "")
		{
			sprintf(s.sysmsg, "原内部钢种不能为空"); //系统错误信息
			strcpy(s.msg, _RES("MMSMS0000107")/*"原内部钢种不能为空!"*/); //用户提示信息，国际化
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		if(c_pono_out == "")
		{
			sprintf(s.sysmsg, "变更制造命令号不能为空"); //系统错误信息
			strcpy(s.msg, _RES("MMSMS0000108")/*"变更制造命令号不能为空!"*/); //用户提示信息，国际化
			throw CApplicationException(-1, s.msg, log.Location); 
		}
  	 
 		if(c_st_no_out == "")
		{
			sprintf(s.sysmsg, "变更内部钢种不能为空"); //系统错误信息
			strcpy(s.msg, _RES("MMSMS0000109")/*"变更内部钢种不能为空!"*/); //用户提示信息，国际化
			throw CApplicationException(-1, s.msg, log.Location); 
		}
 	 
		recReviseTime = CDateTime::Now().ToString("yyyyMMddHHmmss"); 
		recRevisor = (CString)s.userid; 

		
	
	 
	  //转炉实绩
	  sqlid = 1;
	  updPono = "PPPPPPP";
	  updStNo = " ";
	  sqlstr = "UPDATE TMMSM21 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_out + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 2;
	  updPono = c_pono_out;
	  updStNo = c_st_no_out;
	  sqlstr = "UPDATE TMMSM21 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_in + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 3;
	  updPono = c_pono_in;
	  updStNo = c_st_no_in;
	  sqlstr = "UPDATE TMMSM21 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = 'PPPPPPP'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();
 		
	  //吹氩实绩
	  sqlid = 4;
	  updPono = "PPPPPPP";
	  updStNo = " ";
	  sqlstr = "UPDATE TMMSM22 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_out + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 5;
	  updPono = c_pono_out;
	  updStNo = c_st_no_out;
	  sqlstr = "UPDATE TMMSM22 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_in + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();
 	 
	  sqlid = 6;
	  updPono = c_pono_in;
	  updStNo = c_st_no_in;
	  sqlstr = "UPDATE TMMSM22 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = 'PPPPPPP'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();
		
 	  //RH实绩
	  sqlid = 7;
	  updPono = "PPPPPPP";
	  updStNo = " ";
	  sqlstr = "UPDATE TMMSM23 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_out + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

 	  sqlid = 8;
	  updPono = c_pono_out;
	  updStNo = c_st_no_out;
	  sqlstr = "UPDATE TMMSM23 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_in + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 9;
	  updPono = c_pono_in;
	  updStNo = c_st_no_in;
	  sqlstr = "UPDATE TMMSM23 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = 'PPPPPPP'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();
   		
 	  //LF实绩
	  sqlid = 10;
	  updPono = "PPPPPPP";
	  updStNo = " ";
	  sqlstr = "UPDATE TMMSM24 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_out + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();
 
	  sqlid = 11;
	  updPono = c_pono_out;
	  updStNo = c_st_no_out;
	  sqlstr = "UPDATE TMMSM24 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_in + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 12;
	  updPono = c_pono_in;
	  updStNo = c_st_no_in;
	  sqlstr = "UPDATE TMMSM24 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = 'PPPPPPP'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();
		
 	  
	  //VD实绩
	  sqlid = 13;
	  updPono = "PPPPPPP";
	  updStNo = " ";
	  sqlstr = "UPDATE tmmsm25 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_out + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

      sqlid = 14;
	  updPono = c_pono_out;
	  updStNo = c_st_no_out;
	  sqlstr = "UPDATE tmmsm25 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_in + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 15;
	  updPono = c_pono_in;
	  updStNo = c_st_no_in;
	  sqlstr = "UPDATE tmmsm25 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = 'PPPPPPP'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();
 		   
 	  //连铸钢水实绩
	  sqlid = 16;
	  updPono = "PPPPPPP";
	  updStNo = " ";
	  sqlstr = "UPDATE TMMSM31 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_out + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 17;
	  updPono = c_pono_out;
	  updStNo = c_st_no_out;
	  sqlstr = "UPDATE TMMSM31 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_in + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 18;
	  updPono = c_pono_in;
	  updStNo = c_st_no_in;
	  sqlstr = "UPDATE TMMSM31 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = 'PPPPPPP'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

 	  //炼钢连铸炉次流实绩表
	/*  sqlid = 19;
	  updPono = "PPPPPPP";
	  updStNo = " ";
	  sqlstr = "UPDATE TMMSM32 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_out + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 20;
	  updPono = c_pono_out;
	  updStNo = c_st_no_out;
	  sqlstr = "UPDATE TMMSM32 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_in + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 21;
	  updPono = c_pono_in;
	  updStNo = c_st_no_in;
	  sqlstr = "UPDATE TMMSM32 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = 'PPPPPPP'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();*/

	  //连铸板坯切割实绩
	  sqlid = 22;
	  updPono = "PPPPPPP";
	  updStNo = " ";
	  sqlstr = "UPDATE TMMSM33 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_out + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 23;
	  updPono = c_pono_out;
	  updStNo = c_st_no_out;
	  sqlstr = "UPDATE TMMSM33 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = '" + c_pono_in + "'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();

	  sqlid = 24;
	  updPono = c_pono_in;
	  updStNo = c_st_no_in;
	  sqlstr = "UPDATE TMMSM33 SET PONO = '" + updPono + "',ST_NO = '" + updStNo + "',REC_REVISOR = '"+ recRevisor + "',REC_REVISE_TIME = '" + recReviseTime + "'"
		         " WHERE PONO = 'PPPPPPP'";
 	  updateCmd.SetCommandText(sqlstr);
	  updateCmd.ExecuteNonQuery();
 		
 	   	  

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
