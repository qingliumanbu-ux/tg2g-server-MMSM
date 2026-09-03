/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     MFJ
Version:    1.0
Date:       2024-02-02
Description: 自循环废钢-手动录入-不发电文-给原料的
**************************************************/
#include "stdafx.h"
//using namespace BM2;
////////  调用原料的函数，原料函数里会反写TMMSM68表，将重量和标记更新调
////////
//////////
//////////
//////////
//////////
//////////

int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
// service入口
BM2F_ENTERACE(mmsm68_pro)

int f_mmsm68_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString	c_datetime("");
	CString v_proc_div = "";//区分标记 
	CString v_resume_seq_no = "";
	CString v_prod_shift_no = "";
	CString v_prod_shift_group = "";
	/* 业务变量 */

	/* 实体类定义 */
	
	CModel    tmmsm68("TMMSM68");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDecimal cd_seq_no = 0;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		//新增
		if (v_proc_div == "I")
		{
			tmmsm68.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			if (tmmsm68["SHIP_NAME"].ToString().Trim() == "")
			{
				sprintf(s.sysmsg, "主车号不可为空，请重新确认数据！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//获取班次班组
			/*if (tmmsm68["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm68["PROD_SHIFT_GROUP"].ToString().Trim() == "")
			{
				f_epep_get_shift_group("SMCP", tmmsm68["PROD_TIME"].ToString(), v_prod_shift_no, v_prod_shift_group, conn);
				tmmsm68["PROD_SHIFT_NO"] = v_prod_shift_no;
				tmmsm68["PROD_SHIFT_GROUP"] = v_prod_shift_group;
			}*/

			doFlag = f_mm0011("TMMSM68_SEQ", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm68["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			tmmsm68["REC_CREATE_TIME"] = datetime;
			tmmsm68["REC_CREATOR"] = s.userid;
			tmmsm68.TrimOrBlank();
			tmmsm68.Insert();

		}
		else if (v_proc_div == "D")
		{
			tmmsm68.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			//当重量大于0或是标记为1，表示已在原料使用，此处不允许删除,先预定9为原料区的逆流程，当为9时可以操作删除
			if (tmmsm68["NET_WT"].ToDecimal() > 0 && tmmsm68["USE_LOGO"].ToString() != "9")
			{
				sprintf(s.sysmsg, "净重大于0，不符合删除条件，请先通知原料区处理！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm68["USE_LOGO"].ToString() == "1")
			{
				sprintf(s.sysmsg, "该数据已在原料区使用，不符合删除条件，请先通知原料区处理！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm68.Delete();
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
