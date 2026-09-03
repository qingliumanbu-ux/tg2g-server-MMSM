/*=========================================================================
//程序名称:     f_caai_getprice
//隶属子系统:   CA
//产品名称:     BM2PES
//创建人员:     ZHOULI
//创建时间:     2012-11-26
//修改人员:     
//修改日期:     
//=========================================================================*/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
BM2_FUNCTION_EXPORT
int f_mmsm_getprice(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int i = 0;
	int doFlag = 0;

	CDecimal use_cr = 0;
	CDecimal use_ni = 0;
	CDecimal use_mo = 0;
	CDecimal price_add_unit = 0;
	CDecimal unit_price = 0;
	CDecimal unit_price_cr = 0;
	CDecimal unit_price_ni = 0;

	CString price_type = " ";

	CString sqlstr = "";
	CModel tqmtscb00_dr("TQMTSCB00_DR");

	// 创建电文处理对象
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{
		tqmtscb00_dr.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		use_cr = bcls_rec->Tables[0].Rows[0]["CR_VALUE"].ToDecimal();
		use_ni = bcls_rec->Tables[0].Rows[0]["NI_VALUE"].ToDecimal();
		use_mo = bcls_rec->Tables[0].Rows[0]["MO_VALUE"].ToDecimal();		
		
		bcls_ret->Tables[0].Clear();
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "UNIT_PRICE");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "UNIT_PRICE_CR");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "UNIT_PRICE_NI");
		bcls_ret->Tables[0].Rows.Add();	 		

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default: // 通用
				sqlstr = "select TYPE_CONVERT,UNIT_PRICE,USE_CR,USE_NI,USE_MO,UNIT_PRICE_CR,UNIT_PRICE_NI"
					" from tqmtscb00_dr t"
					" WHERE 1=1 "
					" and PRICE_TYPE = @price_type"
					" AND t.mat_code = @mat_code "
					" order by DATE_C desc"
					;

				break;
			}
			//Log::Trace("", "", "mat_code={0}", tqmtscb00_dr["MAT_CODE"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code", tqmtscb00_dr["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("price_type", tqmtscb00_dr["PRICE_TYPE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				unit_price = cmd_inq.GetDecimal(2);
				if (cmd_inq.GetString(1) == "单项")
				{
					if (cmd_inq.GetDecimal(3) != 0)
					{
						//取铬值
						price_add_unit = 0;
						price_add_unit = (cmd_inq.GetDecimal(2) / cmd_inq.GetDecimal(3)).Round(2);
						unit_price = unit_price + (price_add_unit*(use_cr - cmd_inq.GetDecimal(3))).Round(2);
					}
					if (cmd_inq.GetDecimal(4) != 0)
					{
						//取镍值
						price_add_unit = 0;
						price_add_unit = (cmd_inq.GetDecimal(2) / cmd_inq.GetDecimal(4)).Round(2);
						unit_price = unit_price + (price_add_unit*(use_ni - cmd_inq.GetDecimal(4))).Round(2);
					}
					if (cmd_inq.GetDecimal(5) != 0)
					{
						//取钼值
						price_add_unit = 0;
						price_add_unit = (cmd_inq.GetDecimal(2) / cmd_inq.GetDecimal(5)).Round(2);
						unit_price = unit_price + (price_add_unit*(use_mo - cmd_inq.GetDecimal(5))).Round(2);
					}
				}
				if (cmd_inq.GetString(1) == "多项" || cmd_inq.GetString(1) == "双折" || cmd_inq.GetString(1) == "三折")
				{
					if (cmd_inq.GetDecimal(3) != 0)
					{
						//取铬值
						price_add_unit = 0;
						sqlstr = " select UNIT_PRICE,USE_CR,USE_NI"
							"  from tqmtscb00_dr "
							" where SUGGEST_CR = '1'"
							" order by DATE_C desc"
							;
						cmd_inq_1.SetCommandText(sqlstr);
						cmd_inq_1.ExecuteReader();
						while (cmd_inq_1.Read())
						{
							price_add_unit = (cmd_inq_1.GetDecimal(1) / cmd_inq.GetDecimal(2)).Round(2);
						}
						cmd_inq_1.Close();
						unit_price = unit_price + (price_add_unit*(use_cr - cmd_inq.GetDecimal(3))).Round(2);
					}
					if (cmd_inq.GetDecimal(4) != 0)
					{
						//取镍值
						price_add_unit = 0;
						sqlstr = " select UNIT_PRICE,USE_CR,USE_NI"
							"  from tqmtscb00_dr "
							" where SUGGEST_NI = '1'"
							" order by DATE_C desc"
							;
						cmd_inq_1.SetCommandText(sqlstr);
						cmd_inq_1.ExecuteReader();
						while (cmd_inq_1.Read())
						{
							price_add_unit = (cmd_inq_1.GetDecimal(1) / cmd_inq_1.GetDecimal(3)).Round(2);
						}
						cmd_inq_1.Close();
						unit_price = unit_price + (price_add_unit*(use_ni - cmd_inq.GetDecimal(4))).Round(2);
					}
					if (cmd_inq.GetDecimal(5) != 0)
					{
						//取镍值
						price_add_unit = 0;
						sqlstr = " select UNIT_PRICE,USE_CR,USE_MO"
							"  from tqmtscb00_dr "
							" where SUGGEST_MO = '1'"
							" order by DATE_C desc"
							;
						cmd_inq_1.SetCommandText(sqlstr);
						cmd_inq_1.ExecuteReader();
						while (cmd_inq_1.Read())
						{
							price_add_unit = (cmd_inq_1.GetDecimal(1) / cmd_inq_1.GetDecimal(3)).Round(2);
						}
						cmd_inq_1.Close();
						unit_price = unit_price + (price_add_unit*(use_mo - cmd_inq.GetDecimal(5))).Round(2);
					}
				}

				unit_price_cr = cmd_inq.GetDecimal(6);
				unit_price_ni = cmd_inq.GetDecimal(7);

			}
			cmd_inq.Close();  	

		bcls_ret->Tables[0].Rows[0]["UNIT_PRICE"] = unit_price;
		bcls_ret->Tables[0].Rows[0]["UNIT_PRICE_CR"] = unit_price_cr;
		bcls_ret->Tables[0].Rows[0]["UNIT_PRICE_NI"] = unit_price_ni;
		//Log::Trace("", "", "unit_price={0}", unit_price);


	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.msg, (const char*)str, sizeof(s.msg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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

	return doFlag;
}
