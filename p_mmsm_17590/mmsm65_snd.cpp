/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167
Version:    1.0
Date:       2023-12-04
Description: MMSM65增删改
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件


//外部函数声明
int f_mmsm_21c001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(mmsm65_snd)

int f_mmsm65_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm65("TMMSM65");
	CModel tmmsm65_1("TMMSM65");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PURCHASEDOCID");
		/* 获得传入参数 */

		
		if (bcls_rec->Tables.IndexOf("MMSM65_SND") >= 0)
		{
			CString vdeal_flag =  bcls_rec->Tables["MMSM65_SND"].Rows[0]["DEAL_FLAG"].ToString();
			for (int i = 0; i < bcls_rec->Tables["MMSM65_SND"].Rows.get_Count(); i++)
			{
				tmmsm65.Reset();
				tmmsm65.MergeFrom(bcls_rec->Tables["MMSM65_SND"].Rows[i]);
				tmmsm65_1.MergeFrom(bcls_rec->Tables["MMSM65_SND"].Rows[i]);

				tmmsm65_1.Query("PURCHASEDOCID,SEQ_NO,MAT_CODE");
				if (vdeal_flag == "I")
				{  
					if (tmmsm65_1["STATUS"].ToString() == "1")
					{
						sprintf(s.msg, "计量单已上传不能再次发送！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (vdeal_flag == "D")
				{
					if (tmmsm65_1["STATUS"].ToString() != "1")
					{
						sprintf(s.msg, "计量单未上传状态，不能撤销！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				tmmsm65["APTIME"] = bcls_rec->Tables["MMSM65_SND"].Rows[i]["APTIME"].ToString().SubstringNE(0,14);
				//tmmsm65.Update("*", "PURCHASEDOCID");
				Log::Trace("", __FUNCTION__, "===tmmsm65[]= [{0}]", tmmsm65["APTIME"].ToString());


				EIClass inBlock_1000;
			
				inBlock_1000.Tables[0].Clear();
				tmmsm65.MergeTo(inBlock_1000.Tables[0]);

				if (!inBlock_1000.Tables[0].Columns.Contains("DEAL_FLAG"))
				{
					inBlock_1000.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
					inBlock_1000.Tables[0].Rows[0]["DEAL_FLAG"] = vdeal_flag;
				} 

				doFlag = f_mmsm_21c001_snd(&inBlock_1000, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c001_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (vdeal_flag == "I") tmmsm65["STATUS"] = "1";
				if (vdeal_flag == "D") tmmsm65["STATUS"] = "2";
				tmmsm65.Update("STATUS","PURCHASEDOCID,SEQ_NO,MAT_CODE");

				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[i]["PURCHASEDOCID"] = tmmsm65["PURCHASEDOCID"];

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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


