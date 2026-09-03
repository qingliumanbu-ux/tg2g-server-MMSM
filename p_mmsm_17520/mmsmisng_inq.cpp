/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 陈芳
日期: 2013-12-25
功能: 按生产日期查询特定炼钢的铁水重量实绩
修改历史：
日期:________；修改人：________; 需求提出人________
变更内容:
**************************************************/  
#include "stdafx.h"
/*<remark>=========================================================
/// <summary>
/// 按产出日期查询特定炼钢的铁水实绩
/// <para>根据炼钢区分查询板坯精整实绩TMMSM0N40/TMMSM0620</para>
/// <param name="mmsm_table_name">满足条件的数据表名</param>
/// <returns>满足查询条件的炼钢铁水重量实绩TMMSM0N40/TMMSM0620数据</returns>
===========================================================</remark>*/
// Service 入口
BM2F_ENTERACE(mmsmisng_inq);
int f_mmsmisng_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__);//系统日志类定义
	/* 程序内部变量 */
	int  doFlag = 0;
	CString  sqlstr(""); //数据库SQL操作字符串，用于捕获数据库操作异常情况
	CDecimal nRecCount = 0;
	CString  sql_select = "";
	CDbCommand cmd_inq(conn);
	CDbCommand comm1(conn);
	CDbCommand comm2(conn);
	CDbCommand comm3(conn);
	CDecimal iron_wt_1 = 0;
	CDecimal iron_wt_2 = 0;

	try
	{
		CString strPosition;	//翻页查询起始位置
	
		CString v_rec_create_t_f = ((CString)bcls_rec->Tables[0].Rows[0]["REC_CREATE_T_F"]).Trim();
        CString v_rec_create_t_e = ((CString)bcls_rec->Tables[0].Rows[0]["REC_CREATE_T_E"]).Trim();
        CString v_mmsm_table_name = ((CString)bcls_rec->Tables[0].Rows[0]["MMSM_TABLE_NAME"]).Trim();
		CString v_div_st = ((CString)bcls_rec->Tables[0].Rows[0]["DIV_ST"]).Trim();
		//分页传入的参数
        int nPageSize = (int)bcls_rec->Tables[1].Rows[0]["PageSize"];
		int nRecordFrom = (int)bcls_rec->Tables[1].Rows[0]["RecordFrom"];
      
		Log::Trace("",__FUNCTION__,"v_div_st = [{0}]",(const char*)v_div_st);
		bcls_ret->Tables[0].set_TableName("MMSMISNG_INQ");

		//炼钢区分输入时计算不同炼钢输出条数和信息
		if(v_rec_create_t_f.Trim()  !="")
		{
			//查询时间不能为空，从传入生产日期前一晚的22：00到传入日期当晚22:00
			sql_select += " where REC_CREATE_TIME between '"+v_rec_create_t_f+"' and '"+v_rec_create_t_e+"'";
			sql_select += " and (TC_PROC_FLAG='2')";

			//如果传入的炼钢区分与连铸机号都不为空时查询输出相应的出库表
			if(v_div_st.Trim() !="")
			{
				if (v_div_st.Trim() == "3")
				{
					sprintf(s.msg,"数据库中没有数据");
				}
				else
				{
					//查询语句
					CString  sqlstr = "select REC_CREATE_TIME AS PRO_DATE,IRON_LADLE_NO,CAST(IRON_WT*0.1 AS DECIMAL(10,2)) AS IRON_WT,CAST(MEASURE_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS MEASURE_EMPTY_WT,CAST(LIFTING_FILLED_WT*0.1 AS DECIMAL(6,2)) AS LIFTING_FILLED_WT, ";
					if(v_mmsm_table_name.Trim().ToUpper() == "TMMSM0N40")
					{
						sqlstr += "TAP_NO AS STEEL_NO,FE_COUNT,TK_NO,CAST(IRON_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS IRON_EMPTY_WT,CAST(MEASURE_FILLED_WT*0.1 AS DECIMAL(6,2)) AS MEASURE_FILLED_WT,CAST(IRON_FILLED_WT*0.1 AS DECIMAL(6,2)) AS IRON_FILLED_WT,SUBSTR(TAP_NO,2,5) AS HEAT_NO ";
					}
					if(v_mmsm_table_name.Trim().ToUpper() == "TMMSM0620")
					{
						sqlstr += "STEEL_NO,0 AS FE_COUNT,' ' AS TK_NO,CAST(IMMIT_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS IRON_EMPTY_WT,0 AS MEASURE_FILLED_WT,CAST(IMMIT_FILLED_WT*0.1 AS DECIMAL(6,2)) AS IRON_FILLED_WT,SUBSTR(STEEL_NO,2,5) AS HEAT_NO ";
					}

					sqlstr +=  " from "+v_mmsm_table_name+" ";      
					Log::Trace("",__FUNCTION__,"sqlstr = [{0}]",(const char*)sqlstr);

					//查询输出总的输出数据条数 
					CString sql_count = "select count(*) from "+ v_mmsm_table_name +" ";
					sql_count += sql_select;
					sqlstr += sql_select;
					sqlstr += " order by HEAT_NO ";  //20140121根据出钢钢号后5位排序
					CDbCommand comm0(sql_count,conn);
					nRecCount = comm0.ExecuteScalar();
					bcls_ret->ExtendedProperties.Add("REC_COUNT", nRecCount.ToString());

					//查询输出表信息
					CDbCommand comm(sqlstr,conn);
					Log::Trace("",__FUNCTION__,"sqlstr = [{0}]",(const char*)sqlstr);
					int count = comm.ExecuteQuery(bcls_ret->Tables[0], nPageSize, nRecordFrom);
					bcls_ret->Tables[0].set_TableName("MMSMISNG_INQ");
				}
			}

			else if (v_div_st.Trim() =="")    //炼钢区分为空时，输出所有炼钢的信息
			{
				CString sqlstr1 = "select  REC_CREATE_TIME AS PRO_DATE,IRON_LADLE_NO,CAST(IRON_WT*0.1 AS DECIMAL(10,2)) AS IRON_WT,CAST(MEASURE_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS MEASURE_EMPTY_WT, "
					              " CAST(LIFTING_FILLED_WT*0.1 AS DECIMAL(6,2)) AS LIFTING_FILLED_WT,TAP_NO AS STEEL_NO,FE_COUNT,TK_NO,CAST(IRON_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS IRON_EMPTY_WT, "
								  " CAST(MEASURE_FILLED_WT*0.1 AS DECIMAL(6,2)) AS MEASURE_FILLED_WT,CAST(IRON_FILLED_WT*0.1 AS DECIMAL(6,2)) AS IRON_FILLED_WT,SUBSTR(TAP_NO,2,5) AS HEAT_NO  from TMMSM0N40 ";
				sqlstr1 += sql_select;		
				Log::Trace("",__FUNCTION__,"sqlstr1= [{0}]",(const char*)sqlstr1);

				sqlstr1 += " UNION ALL "
				           " select  REC_CREATE_TIME AS PRO_DATE,IRON_LADLE_NO,CAST(IRON_WT*0.1 AS DECIMAL(10,2)) AS IRON_WT,CAST(MEASURE_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS MEASURE_EMPTY_WT, "
						   " CAST(LIFTING_FILLED_WT*0.1 AS DECIMAL(6,2)) AS LIFTING_FILLED_WT,STEEL_NO,0 AS FE_COUNT,' ' AS TK_NO,CAST(IMMIT_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS IRON_EMPTY_WT, "
						   " 0 AS MEASURE_FILLED_WT,CAST(IMMIT_FILLED_WT*0.1 AS DECIMAL(6,2)) AS IRON_FILLED_WT,SUBSTR(STEEL_NO,2,5) AS HEAT_NO from TMMSM0620 ";
				sqlstr1 += sql_select;
				Log::Trace("",__FUNCTION__,"sqlstr2= [{0}]",(const char*)sqlstr1);		
	     
				sqlstr = " SELECT * FROM ( "  ;
				sqlstr += sqlstr1 ;
				sqlstr +=  " )order by HEAT_NO ";  //20140121根据出钢钢号后5位排序
				//查询输出总的输出数据条数
				CString sql_count = "select count(*) from ( "+ sqlstr1 +" )";
				CDbCommand comm0(sql_count,conn);
				nRecCount = comm0.ExecuteScalar();
				bcls_ret->ExtendedProperties.Add("REC_COUNT", nRecCount.ToString());
				Log::Trace("",__FUNCTION__,"nRecCount = 【{0}】",nRecCount);

				//查询输出表信息
				CDbCommand comm1(sqlstr,conn);
				Log::Trace("",__FUNCTION__,"sqlstr= [{0}]",(const char*)sqlstr);
				int count1 = comm1.ExecuteQuery(bcls_ret->Tables[0], nPageSize, nRecordFrom);
				bcls_ret->Tables[0].set_TableName("MMSMISNG_INQ");
				nRecCount = count1;
				Log::Trace("",__FUNCTION__,"共查询了count1 = [{0}]条数据",count1);
			}
			if(nRecCount==0)
			{
				sprintf(s.msg,"数据库中没有数据");
			}
			else
			{
				sprintf(s.msg,"查询成功！");
			}
		}
       
	}
    catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);	//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(const CApplicationException& ex)//捕获应用错误
	{
		doFlag = ex.GetCode();
		strcpy(s.msg, (const char*)ex.GetMsg());
	}
	catch(const CException& ex)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)ex.GetMsg());
	}
	cmd_inq.Close();		
	s.flag = doFlag;

	return(doFlag);
}
