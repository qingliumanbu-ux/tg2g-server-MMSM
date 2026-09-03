/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		MFJ
Version:    1.0
Date:		2024-01-18
Description:钢坯初判等级 发送L4电文
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_210046_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = " ";
	CString v_mat_no = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	CString cs_tc_no = "";//电文号
	EPEX epex;

	/* 实体类定义 */
	CModel tmmsm3f("TMMSM3F");


	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("210046");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 210046 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		for (int i = 0; i < bcls_rec->Tables["210046"].Rows.get_Count(); i++)
		{
			cs_tc_no = "210046";
			//电文初始化
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				Log::Trace("", __FUNCTION__, "电文初始化失败");
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}

			tmmsm3f.Reset();//将数据清空
			tmmsm3f.MergeFrom(bcls_rec->Tables["210046"].Rows[i]);//获取数据
			tmmsm3f.Query();

			//拼接材料号
			CString slabNo = tmmsm3f["SLAB_NO"].ToString();
			CString matNo;
			matNo = slabNo.Substring(0, 8) + slabNo.Substring(15, 2);

			Log::Trace("", __FUNCTION__, "拼接matNo[{0}]  ", matNo);

			if (epex.SetValue(0, tmmsm3f) < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (epex.SetValue("bapiheader", "msgtype", 0, cs_tc_no) < 0
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}



			if (epex.SetValue("tmmmat_level", "mat_no", 0, matNo) < 0  //材料号
				|| epex.SetValue("tmmmat_level", "mat_level", 0, tmmsm3f["CK_RESULT"].ToString()) < 0  //材料等级
				|| epex.SetValue("tmmmat_level", "mat_level_reason", 0, " ") < 0  //等级原因
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}




			//电文发送
			if (epex.SendTele() < 0)
			{
				strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else
			{
				Log::Trace("", __FUNCTION__, "发送电文成功");
			}
			epex.Uninitialize();
		}



	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
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

	return doFlag;

}
