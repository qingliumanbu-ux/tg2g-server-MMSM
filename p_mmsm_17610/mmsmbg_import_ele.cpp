/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     王建征
Version:    1.0
Date:       2020-07-14
Description: 炼钢期初成分数据导入
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢期初成分数据导入
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  
//#include "tqmtqb0.h" 

//外部函数声明
BM2_FUNCTION_EXPORT
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号

BM2F_ENTERACE(mmsmbg_import_ele)

int f_mmsmbg_import_ele(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");
	CString cs_heat_no = "";

	/* 实体类定义 */ 	
	CModel tqmtqq0("TQMTQQ0");
	CModel tqmtqb0("TQMTQB0");
	

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	/* 添加并设置块名 */
	blkNum = bcls_rec->Tables.IndexOf("MM0099");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099");
	}

	try
	{
		//获取系统当前时刻
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//获取输入参数
			cs_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();

			//打印输入参数
			Log::Trace("", __FUNCTION__, "cs_heat_no = [{0}]", cs_heat_no);

			//B0表
			//重置结构体待用
			tqmtqb0.Reset();
			tqmtqb0["REC_CREATOR"] = "QC";
			tqmtqb0["REC_CREATE_TIME"] = datetime;
			tqmtqb0["HEAT_NO"] = cs_heat_no;
			tqmtqb0["PONO"] = cs_heat_no;

			tqmtqb0["PCH_JUDGE_CODE"] = "1";//写死

			//期初数据验证是否存在，存在的跳过
			if (tqmtqb0.QueryCount("HEAT_NO")>0)
			{
				Log::Trace("", __FUNCTION__, "炉号 = [{0}]已存在，跳过", cs_heat_no);
				continue;
			}
			
			tqmtqb0.Insert();
			
			//Q0表
			for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
			{
				if (bcls_rec->Tables[0].Columns[j].get_ColumnName() != "HEAT_NO")
				{
					//if ((CDecimal)bcls_rec->Tables[0].Rows[0][j]!=0)
					//{
						Log::Trace("", __FUNCTION__, "元素代码 = [{0}]", bcls_rec->Tables[0].Columns[j].get_ColumnName());
						//重置结构体待用
						tqmtqq0.Reset();

						//Q0表
						tqmtqq0["REC_CREATOR"] = "QC";
						tqmtqq0["REC_CREATE_TIME"] = datetime;
						tqmtqq0["HEAT_NO"] = cs_heat_no;
						tqmtqq0["PONO"] = cs_heat_no;

						tqmtqq0["ELM_CODE"] = bcls_rec->Tables[0].Columns[j].get_ColumnName();
						tqmtqq0["ELM_NAME"] = " ";//最后统一刷
						tqmtqq0["ELM_POS"] = 0;//最后统一刷
						tqmtqq0["ELM_ACT"] = (CDecimal)bcls_rec->Tables[0].Rows[0][j];
						tqmtqq0["ELM_OK"] = 9;//写死

						tqmtqq0.Insert();
					//}
				}				
			}

			//刷新元素名称及顺序
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "UPDATE tqmtqq0 A "
					" SET(ELM_NAME, ELM_POS) = "
					" (SELECT CODE_DESC_1_CONTENT, TO_NUMBER(CODE_DESC_2_CONTENT) "
					" FROM(select CODE, CODE_DESC_1_CONTENT, CODE_DESC_2_CONTENT "
					" from tep0002 "
				" where code_class = 'QMYS') B "
				" WHERE A.ELM_CODE = B.CODE) "
				" where exists(select 1 from(select CODE, CODE_DESC_1_CONTENT, CODE_DESC_2_CONTENT from tep0002 where code_class = 'QMYS') b "
				" where b.CODE = a.ELM_CODE) and ELM_NAME = ' ' ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}
	   
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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
		
