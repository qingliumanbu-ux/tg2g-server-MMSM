/*========================================================================*/
/*== [service名  ]:  mmsm50a_pro      ||  [对应VC#画面 ]:  ALL           ==*/
/*== [程序编制人 ]:  向萍             ||  [程序定稿日期]:2016-2-4 14:00:05==*/
/*== [程序修改人 ]：                  ||  [程序修改日期]:               ==*/
/*========================================================================*/
/*== [数据库表   ]： tmmsm50a                                            ==*/
/*== [调用函数   ]： 无				                                    ==*/
/*== [service功能]： 原辅料按炉消耗信息查询画面需要显示的原辅料字段     ==*/
/*========================================================================*/


/******框架头******/
#include "stdafx.h"


/******业务头******/ 



/******service入口******/
BM2F_ENTERACE(mmsm50a_pro)

int f_mmsm50a_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm50a_pro";                //定义函数英文名称  
	CString FunctionCname = "原辅料信息_显示工序信息后备";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;
	int   v_seq_no = 0;

	CString sqlstr = "";
	CString v_proc_div = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	



	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm50a("TMMSM50A");

		//初始化实体类
		tmmsm50a.Reset();

		/* 获取输入参数*/
	
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		//Log::Trace(" ", __FUNCTION__, "v_proc_div =[{0}]", v_proc_div);

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm50a.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm50a.TrimOrBlank();

			//Log::Trace(" ", __FUNCTION__, "v_mat_code =[{0}]", tmmsm50a["MAT_CODE"].ToString());
			//Log::Trace(" ", __FUNCTION__, "v_mat_name =[{0}]", tmmsm50a["MAT_NAME"].ToString());
			//Log::Trace(" ", __FUNCTION__, "v_station_id =[{0}]", tmmsm50a["STATION_ID"].ToString());
			//Log::Trace(" ", __FUNCTION__, "v_view_flag =[{0}]", tmmsm50a["VIEW_FLAG"].ToString());

			if (tmmsm50a["MAT_CODE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "物料代码不能为空"); //系统错误信息
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (v_proc_div == "I")
			{
				tmmsm50a["REC_CREATE_TIME"] = dateNow;
				tmmsm50a["REC_CREATOR"] = s.userid;
				tmmsm50a.Insert();
			}
			else if (v_proc_div == "U")
			{

				tmmsm50a.Delete("mat_code,station_id");
				tmmsm50a.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsm50a.Delete("mat_code,station_id");
			}

			else if (v_proc_div == "S")//调序
			{
				if (tmmsm50a["VIEW_FLAG"].ToString() == '1')
				{
					v_seq_no++;

					tmmsm50a["SEQ_NO"] = v_seq_no;
				}
				else
				{
					tmmsm50a["SEQ_NO"] = 0;
				}

				tmmsm50a.Update("seq_no", "mat_code,station_id");

			}
		}

	

		/*设置系统返回参数*/
		strcpy(s.msg, _RES("GCRSS0000002"));//处理成功。  

	}



	/*捕获数据库操作异常*/
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

}
