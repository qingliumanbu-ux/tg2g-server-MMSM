//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsmfptg_inexl)

int f_mmsmfptg_inexl(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");



	/* 业务变量 */
	CModel tmmsmfp_tg("TMMSMFP_TG");


	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_inq(conn);
	CDecimal cd_count = 0;
	/* 数据库操作类定义 */

	try
	{

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tmmsmfp_tg.Reset();
			tmmsmfp_tg.MergeFrom(bcls_rec->Tables[0].Rows[i]);


			if (tmmsmfp_tg.QueryCount("HEAT_NO1")<1)
			{
				tmmsmfp_tg["REC_CREATOR"] = s.userid;
				tmmsmfp_tg["REC_CREATE_TIME"] = datetime;
				tmmsmfp_tg["REC_REVISOR"] = " ";
				tmmsmfp_tg["REC_REVISE_TIME"] = " ";
				tmmsmfp_tg.TrimOrBlank();
				tmmsmfp_tg.Insert();
			}
			else
			{
				tmmsmfp_tg["REC_REVISOR"] = s.userid;
				tmmsmfp_tg["REC_REVISE_TIME"] = datetime;
				tmmsmfp_tg.TrimOrBlank();
				tmmsmfp_tg.Update("*", "HEAT_NO1");

			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
