/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2024-02-20
Description: 铁水分配信息操作函数
***********************************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

//接口头文件
#include "x21b003TSDK.h"

int f_getSeqNextValue(CString SEQ_NAME, CString& SEQ_VALUE, CDbConnection * conn);
int f_21b003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm11_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	CString v_pract_coll_mode = "";
	CString v_factory_div = "";
		
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm11("TMMSM11");
	CModel tmmsm11_old("TMMSM11");
	C21B003TSDK X21b003(conn);
 
 	try
	{
		v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString();
		v_pract_coll_mode = bcls_rec->Tables["PARA"].Rows[0]["PRACT_COLL_MODE"].ToString();
		v_factory_div = bcls_rec->Tables["PARA"].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm11.Reset();
			tmmsm11.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm11["FACTORY_DIV"] = v_factory_div;
			tmmsm11.TrimOrBlank();

			if (v_proc_div == "I")				//新增
			{
				if (!tmmsm11.Query("TPC_ID")){
					tmmsm11["REC_CREATE_TIME"] = dateNow;
					tmmsm11["REC_CREATOR"] = s.userid;
					tmmsm11["COMPANY_CODE"] = "TG";
					tmmsm11["COMPANY_NAME"] = "TG";
					CString seqValue = "";
					if (f_getSeqNextValue("TI_CODE_SEQ", seqValue, conn) != 0){
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tmmsm11["TICODE"] = seqValue;
					tmmsm11["RETURN_FLAG"] = 0;
					tmmsm11["EMPTY_FLAG"] = "0";
					tmmsm11["RECV_FLAG"] = "0";
					tmmsm11["ZL_SEND_FLAG"] = "0";

					tmmsm11.Insert();
				}
				else{
					strcpy(s.msg, "罐次号[" + tmmsm11["TPC_ID"].ToString() + "]的记录已存在！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (v_proc_div == "D")				//删除
			{
				tmmsm11.Delete();
			}

			//收料
			else if(v_proc_div == "R")
			{
				tmmsm11_old["TICODE"] = tmmsm11["TICODE"];						//通过调拨单号获取罐次对象

				//如果罐次信息存在
				if (tmmsm11_old.Query())
				{
					//罐次修改、接收信息
					tmmsm11_old["REC_REVISOR"] = s.userid;
					tmmsm11_old["REC_REVISE_TIME"] = dateNow;
					tmmsm11_old["RECV_BY"] = s.userid;
					tmmsm11_old["RECV_TIME"] = dateNow;

					if (tmmsm11["RECV_FLAG"].ToString() == "0")							//预计倒空
					{
						tmmsm11_old["RECV_FLAG"] = "1";
						tmmsm11_old["EMPTY_TIME"] = tmmsm11["EMPTY_TIME"];
						tmmsm11_old.Update("REC_REVISOR, REC_REVISE_TIME, RECV_FLAG, RECV_BY, RECV_TIME, EMPTY_TIME");

						//收料实绩接口对象整理
						X21b003.WEIGH_TIME = tmmsm11_old["EMPTY_TIME"];
					}
					else if (tmmsm11["RECV_FLAG"].ToString() == "1")					//倒空
					{
						tmmsm11_old["RECV_FLAG"] = "2";
						tmmsm11_old["EMPTY_FLAG"] = "1";
						tmmsm11_old["EMPTY_TIME_ACT"] = tmmsm11["EMPTY_TIME_ACT"];
						tmmsm11_old.Update("REC_REVISOR, REC_REVISE_TIME, RECV_FLAG, RECV_BY, RECV_TIME, EMPTY_FLAG, EMPTY_TIME_ACT");
						
						//收料实绩接口对象整理
						X21b003.WEIGH_TIME = tmmsm11_old["EMPTY_TIME_ACT"];
						X21b003.EMPTY_SIGN = "E";
					}
					else if (tmmsm11["RECV_FLAG"].ToString() == "2")					//返重
					{
						tmmsm11_old["RECV_FLAG"] = "3";
						tmmsm11_old["TPC_WT"] = tmmsm11["TPC_WT"];
						tmmsm11_old["ACCOUNT_WT"] = tmmsm11["ACCOUNT_WT"];
						tmmsm11_old["FG_WT_LG"] = tmmsm11["FG_WT_LG"];
						tmmsm11_old.Update("REC_REVISOR, REC_REVISE_TIME, RECV_FLAG, RECV_BY, RECV_TIME, TPC_WT, ACCOUNT_WT, FG_WT_LG");
					
						//收料实绩接口对象整理
						X21b003.NET_WT = tmmsm11_old["TPC_WT"];
						X21b003.SETTLEMENT_WT = tmmsm11_old["ACCOUNT_WT"];
						X21b003.GROSS_WT = tmmsm11_old["FG_WT_LG"];
						X21b003.BACK1 = tmmsm11_old["TICODE"];
					}
					else if (tmmsm11["RECV_FLAG"].ToString() == "3")					//到达温度&时间
					{
						tmmsm11_old["RECV_FLAG"] = "5";
						tmmsm11_old["IRON_TEMP"] = tmmsm11["IRON_TEMP"];
						tmmsm11_old["TIME_TORPEDO_IN"] = tmmsm11["TIME_TORPEDO_IN"];
						tmmsm11_old.Update("REC_REVISOR, REC_REVISE_TIME, RECV_FLAG, RECV_BY, RECV_TIME, IRON_TEMP, TIME_TORPEDO_IN");
					
						//收料实绩接口对象整理
						X21b003.BACK3 = tmmsm11_old["IRON_TEMP"].ToString().Substring(1, 4);
						X21b003.BACK5 = tmmsm11_old["TIME_TORPEDO_IN"];
					}
					else if (tmmsm11["RECV_FLAG"].ToString() == "5")					//确认
					{
						tmmsm11_old["RECV_FLAG"] = "6";
						tmmsm11_old.Update("REC_REVISOR, REC_REVISE_TIME, RECV_FLAG, RECV_BY, RECV_TIME");
					}

					//发送铁区MES收料实绩
					if (tmmsm11_old["RECV_FLAG"].ToString() != "6")
					{
						//收料实绩接口对象整理
						X21b003.DEAL_FLAG = tmmsm11_old["RECV_FLAG"];
						X21b003.WORK_DATE = CDateTime::Now().ToString("yyyyMMdd");
						X21b003.TCP_NO = tmmsm11_old["TAPNO"];
						X21b003.TPC_SEQ = tmmsm11_old["TPC_ID"];
						X21b003.TPC_NO = tmmsm11_old["TPC_YL_NO"];

						//收料实绩Block定义
						EIClass inBlock;
						inBlock.Tables.Add();
						X21b003.SetSchema(inBlock.Tables[0], true);
						inBlock.Tables[0].Rows.Add();
						inBlock.Tables[0].Rows[0].Merge(X21b003);

						if (f_21b003_snd(&inBlock, bcls_ret, conn))
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}
			}
		}
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
