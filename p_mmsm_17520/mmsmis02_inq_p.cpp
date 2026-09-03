/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 吴振楠
日期: 2012-2-17
功能: 按CAST_LOT_NO查询前后PONO
修改历史：
 日期:________；修改人：________; 需求提出人________
 变更内容:
**************************************************/  
#include "stdafx.h"
#include "tmmsmis1f.h"  //业务头
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

/*<remark>=========================================================
/// <summary>
/// 物料信息查询
/// <para>如果查询在线数据，根据查询条件查询冷轧物料主表TMMCR01</para>
/// <para>如果查询历史数据，根据查询条件查询冷轧物料历史表HMMCR01</para>
/// </summary>
/// <param name="CRRNT_HSTY_FLAG">在线档OR历史档标记</param>
/// <param name="MAT_NO">材料号</param>
/// <param name="FACTORY_DIV">厂别区分</param>
/// <param name="NEXT_UNIT_CODE">下道机组代码</param>
/// <param name="...">其他...</param>
/// <returns>满足查询条件的冷轧物料主表TMMCR01数据</returns>
===========================================================</remark>*/
BM2F_ENTERACE(mmsmis02_inq_p)

int f_mmsmis02_inq_p(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);//系统日志类定义
	
	/* 程序内部变量 */
	int doFlag = 0;
	int sql_flag = 0;
	int i = 0;

	CString sqlstr = " ";
	int	blkNum1	= 0;
	int	blkNum	= 0;
	/* 实体类定义 */
	CTMMSMIS1F tmmsmis1f(conn);
	try
	{
		CDbCommand cmdcount(conn);
		CDbCommand cmdinq(conn);
		
		CString strSql1 = "";
		CString strSql2 = "";
		CString o_pono ="";
		CDecimal v_query_pflag = bcls_rec->Tables[0].Rows[0]["query_pflag"];
		CString v_cast_no = bcls_rec->Tables[0].Rows[0]["cast_no"];
		CDecimal v_cast_div_no = bcls_rec->Tables[0].Rows[0]["cast_div_no"];
		CString v_pono = bcls_rec->Tables[0].Rows[0]["pono"];
		
		Log::Trace("",__FUNCTION__,"v_query_pflag = 【{0}】",v_query_pflag.ToInt32());
		Log::Trace("",__FUNCTION__,"v_cast_no = 【{0}】",(const char*)v_cast_no);
		Log::Trace("",__FUNCTION__,"v_cast_div_no = 【{0}】",v_cast_div_no.ToInt32());
		/*查询炉次命令信息 传入块1*/
		bcls_ret->Tables[0].set_TableName("TMMSMIS1F1"); 
		bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"PONO");
		
		
		
		/*根据标记查询在线档或历史档  分页*/
		if(v_query_pflag == 2)
		{
			/*数据校验*/
			if(v_cast_no.Trim().Compare(" ")<=0)
			{
				//throw CApplicationException(_RES("MMSMS0000228")/*cast_lot_no不能为空*/);
				sprintf(s.msg, "cast_no不能为空");
				s.flag = -1;
				throw CApplicationException(-1, s.msg, s.svc_name);
				Log::Trace("",__FUNCTION__,"v_cast_no 【{0}】不能为空",(const char*)v_cast_no);

			}
			if(v_cast_div_no<0)
			{
				sprintf(s.msg, "v_cast_div_no不能为空");
				s.flag = -1;
				throw CApplicationException(-1, s.msg, s.svc_name);
				Log::Trace("",__FUNCTION__,"v_cast_div_no 【{0}】不能为空",v_cast_div_no.ToInt32());
				//throw CApplicationException(_RES("MMSMS0000229")/*cc_num不能为空*/);
			}
			strSql1 = " SELECT PONO FROM TMMSMIS1F  "
			          " where cast_no = @v_cast_no and cast_div_no < @v_cast_div_no  order by cast_div_no desc  ";
		}
		else if(v_query_pflag == 3)
		{
			/*数据校验*/
			if(v_cast_no.Trim().Compare(" ")<=0)
			{
				//throw CApplicationException(_RES("MMSMS0000228")/*cast_lot_no不能为空*/);
				sprintf(s.msg, "cast_no不能为空");
				s.flag = -1;
				throw CApplicationException(-1, s.msg, s.svc_name);
				Log::Trace("",__FUNCTION__,"v_cast_no 【{0}】不能为空",(const char*)v_cast_no);

			}
			if(v_cast_div_no<0)
			{
				sprintf(s.msg, "v_cast_div_no不能为空");
				s.flag = -1;
				throw CApplicationException(-1, s.msg, s.svc_name);
				Log::Trace("",__FUNCTION__,"v_cast_div_no 【{0}】不能为空",v_cast_div_no.ToInt32());
				//throw CApplicationException(_RES("MMSMS0000229")/*cc_num不能为空*/);
			}
			strSql1 = " SELECT PONO FROM TMMSMIS1F  "
				      " where cast_no = @v_cast_no and cast_div_no > @v_cast_div_no  order by cast_div_no asc  ";
		}
		else if(v_query_pflag == 0)
		{
			strSql1 = " SELECT PONO FROM TMMSMIS1F  "
				      " where PONO < @pono  order by PONO DESC  ";
		}
		else if(v_query_pflag == 1)
		{
			strSql1 = " SELECT PONO FROM TMMSMIS1F  "
				      " where PONO > @pono  order by PONO ASC  ";
		}
		Log::Trace("",__FUNCTION__,"strSql1 = 【{0}】",(const char*)strSql1);	
		cmdinq.SetCommandText(strSql1);
		cmdinq.Parameters.Clear();
		cmdinq.Parameters.Set("v_cast_div_no",v_cast_div_no);
		cmdinq.Parameters.Set("v_cast_no",v_cast_no);
		cmdinq.Parameters.Set("pono",v_pono);
		
		cmdinq.ExecuteQuery(bcls_ret->Tables["TMMSMIS1F1"],0 ,1);
		
		if(bcls_ret->Tables[0].Rows.get_Count()==0&& (v_query_pflag==0||v_query_pflag==2))
		{
			//Log::Trace("",__FUNCTION__,"没有查询到比 【{0}】 - 【{1}】更小的任何PONO记录",(const char*)v_cast_no,v_cast_div_no.ToInt32());
			//CFormattable arguments[] = {v_cast_no,v_cast_div_no};
			//CMessageFormat::Format(s.msg,_RES("MMSMS0000230")/*没有查询到比 {0} {1}更大的PONO记录*/,arguments, 2);
			sprintf(s.msg, "没有查询到比 【{0}】 - 【{1}】更小的任何PONO记录",(const char*)v_cast_no,v_cast_div_no.ToInt32());
            Log::Trace("",__FUNCTION__,"s.msg = 【{0}】",s.msg);
			
			throw CApplicationException(s.msg);
		}
		if(bcls_ret->Tables[0].Rows.get_Count()==0&&(v_query_pflag==1||v_query_pflag==3))
		{
			/*Log::Trace("",__FUNCTION__,"没有查询到比 【{0}】 - 【{1}】更大的任何PONO记录",(const char*)v_cast_no,v_cast_div_no.ToInt32());
			CFormattable arguments[] = {v_cast_no,v_cast_div_no};
			CMessageFormat::Format(s.msg,_RES("MMSMS0000231"), arguments, 2);*/
			sprintf(s.msg, "没有查询到比 【{0}】 - 【{1}】更大的任何PONO记录",(const char*)v_cast_no,v_cast_div_no.ToInt32());
			Log::Trace("",__FUNCTION__,"s.msg = 【{0}】",s.msg);
			throw CApplicationException(s.msg);
			
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
		strcpy(s.sysmsg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return(doFlag);
}
