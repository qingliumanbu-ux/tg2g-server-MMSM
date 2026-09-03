/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-09-01
Deshription: 炼钢钢坯材料在制品转成品
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料在制品转成品
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  


//外部函数声明
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
int f_t82302_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送智慧质量表面电文
BM2F_ENTERACE(mmsm01a1f8_pro)                                               

int f_mmsm01a1f8_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	int t82302_count = 0;
	/* 业务变量 */
	CString	datetime("");	

	/* 实体类定义 */ 
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099"); 
		}

		//发送智慧质量-板坯表面电文
		EIClass bcls_rec_T82302;
		bcls_rec_T82302.Tables[0].Columns.Add(tmmsm01);
		bcls_rec_T82302.Tables[0].Columns.Add(DT_STRING, "T82302");
		
		/* 获取输入参数 */
		for(int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", __FUNCTION__, "进来了= [{0}]");

			if (tmmsm01.QueryCount("MAT_NO") > 0)
			{
				tmmsm01.Update("SURF_QUALITY,REMARK,SPECIFICATIONS,ZL_REASON_DESC,JUDGE_RESULT_1,TRIMTEXT,CASTING_PRE_JUDGMENT,SLAB_STORAGE_TYPE,OFFLINE_REASON","MAT_NO");
			}
			else
			{
				hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				if (hmmsm01.QueryCount("MAT_NO") > 0)
				{
					hmmsm01.Update("SURF_QUALITY,REMARK,SPECIFICATIONS,ZL_REASON_DESC,JUDGE_RESULT_1,TRIMTEXT,CASTING_PRE_JUDGMENT,SLAB_STORAGE_TYPE,OFFLINE_REASON", "MAT_NO");
				}
				else
				{
					strcpy(s.msg, "在库和历史表中都没有该数据！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}

			bcls_rec_T82302.Tables[0].Rows.Add();
			bcls_rec_T82302.Tables[0].Rows[t82302_count].Merge(tmmsm01);
			bcls_rec_T82302.Tables[0].Rows[t82302_count]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec_T82302.Tables[0].Rows[t82302_count]["T82302"] = "1";
			t82302_count++;

		}


		#pragma region 调用函数，发送智慧质量电文
		if (bcls_rec_T82302.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_t82302_snd(&bcls_rec_T82302, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion
		

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GHRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
