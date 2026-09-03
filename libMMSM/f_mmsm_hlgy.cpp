
/// <summary>
/// 功能说明:根据回炉信息，将实绩的重量及过钢量插入到表里，原炉号扣除，新炉号新增
/// </summary> 

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT	
int f_mmsm_hlgy(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0; 	
	CString heat_no = " "; 	
	CString ret_heat_no = " ";
	CString sqlstr = " ";
	CDecimal all_wt = 0;
	
	

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	CModel tpssm35("TPSSM35");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsmhl("TMMSMHL");
	
	try
	{
		//
		//判断是回炉调还是工艺路线确认调,1表示回炉，2表示工艺路线
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tpssm35.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			//将工艺路径的的信息插入
			sqlstr = " delete tmmsmgy07"
				" where ret_heat_no =@ret_heat_no"
				" and heat_no=@heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("ret_heat_no", tpssm35["RET_HEAT_NO"].ToString());
			cmd_inq_1.Parameters.Set("heat_no", tpssm35["HEAT_NO"].ToString());
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close();

			sqlstr = " insert into tmmsmgy07(REC_CREATOR,REC_CREATE_TIME,heat_no,RET_HEAT_NO,proc_no,dev_code,DURATION_TIME,HANDLE_DIV)"
				" select @rec_creator,@rec_create_time,t1.heat_no,t1.RET_HEAT_NO,t2.l2_proc_no,t2.dev_code,round(t1.rate*t2.DURATION_TIME,0),'H'"
				" from tpssm35 t1 left join tmmsmgy06 t2 on t1.heat_no=t2.heat_no"
				" where 1=1"
				" and t1.ret_heat_no !=' ' "
				" and t1.ret_heat_no = @ret_heat_no"
				" and heat_no = @heat_no"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("ret_heat_no", tpssm35["RET_HEAT_NO"].ToString());
			cmd_inq_1.Parameters.Set("heat_no", tpssm35["HEAT_NO"].ToString());
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close(); 	
			
		}
		
		
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


