/*========================================================================*/
/*== [service名  ]:  mmsmpara_ed54      ||  [对应VC#画面 ]:  ALL        ==*/
/*== [程序编制人 ]:  向萍             ||  [程序定稿日期]:2016-2-4 14:00:05==*/
/*== [程序修改人 ]：                  ||  [程序修改日期]:               ==*/
/*========================================================================*/
/*== [数据库表   ]： tpssmd1                                            ==*/
/*== [调用函数   ]： 无				                                    ==*/
/*== [service功能]： 生产实绩统计项_ED54列信息初始化        ==*/
/*========================================================================*/


/******框架头******/
#include "stdafx.h"

/******业务头******/ 



//从字符串中根据指定分隔符拆分数据
// 入口字符，分隔字符，函数是返回字符信息。
CString f_mmsm_get_multi_value(CString v_in_str,CString v_spilit_flag, CDbConnection * conn); 

/******service入口******/
BM2F_ENTERACE(mmsmpara_ed54)

int f_mmsmpara_ed54(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsmpara_ed54";                //定义函数英文名称  
	CString FunctionCname = " 生产实绩统计项_ED54列信息初始化";   //定义函数中文名称


	/*定义程序用变量*/
	int fetchRowCount = 0;
	int doFlag = 0;
	CString  function_id = "MMSM62_INQ" ;/*自定义显示项目号*/
	CString v_userid = s.userid;

	/*定义业务用变量*/
	CString v_prod_dif = "";
	 
 	CString v_dev_code = ""; //设备代码
	CString v_station_name = ""; //设备名称
	CString v_factory_div = "";

	CString v_factory_div_code = "";
	CString v_factory_div_name = "";

	CString v_column_name = "";
	CString v_column_cname = "";

	CString sqlstr = "";
	CString sqlwhere = "";
	int     v_total_count  = 0;
	int     j = 0;


	/****** 业务处理开始 ******/
	try
	{	
		 
		/*实体类定义*/

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CString  c_sql_where = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition = "";
		CString  c_sql_orderBY = "   ";

		/* 数据库操作类定义2 */
		CDbCommand cmd_sql2(conn); //与DB 建立连接。
		CString  c_sql_where2 = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition2 = "";



		/* 数据库操作类定义3 */
		CDbCommand cmd_sql3(conn); //与DB 建立连接。
		CString  c_sql_where3 = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition3 = "";


		//测试阶段，控制可操作人员。
	/*	if (v_userid.Trim() != "178041")
		{
			sprintf(s.msg,"您的帐号[%s]，暂不支持当前操作。",(const char*)v_userid);
			throw CApplicationException(-1, s.msg, FunctionEname);
		}*/
 
		//将TPSSMD1表中的设备代码，追加到功能号 = MMSM62_INQ中去。
		//==================================================
		CDecimal v_cnt = 0;
		CString v_func_id = ""; 
		
		CString v_class_code = "";
		CDecimal v_seq_no = 0;
		CString v_item_ename = "";
		CString v_item_cname = "";
		CString v_item_must_flag = "";
		CString v_item_type = "";
		CString v_item_len = "";
		CString v_culture = "";
		CString v_area_id = "";
		CString v_station_id = "";
		CString v_wt_name = "";


		Log::Trace(" ", __FUNCTION__, "bcls_rec->Tables[0].Rows.get_Count() =[{0}]", bcls_rec->Tables[0].Rows.get_Count());
	
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			/* 获取输入参数*/

			if (bcls_rec->Tables[0].Rows[i]["PARA_NAME"].ToString().Trim() == "area_id")
			{
				v_area_id = bcls_rec->Tables[0].Rows[i]["PARA"].ToString().Trim();
			}
			if (bcls_rec->Tables[0].Rows[i]["PARA_NAME"].ToString().Trim() == "station_id")
			{
				v_station_id = bcls_rec->Tables[0].Rows[i]["PARA"].ToString().Trim();
			}

			if (bcls_rec->Tables[0].Rows[i]["PARA_NAME"].ToString().Trim() == "func_id_h")
			{
				v_func_id = bcls_rec->Tables[0].Rows[i]["PARA"].ToString().Trim();
			}
			if (bcls_rec->Tables[0].Rows[i]["PARA_NAME"].ToString().Trim() == "wt_name")
			{
				v_wt_name = bcls_rec->Tables[0].Rows[i]["PARA"].ToString().Trim();
			}
		
			Log::Trace(" ", __FUNCTION__, "v_area_id =[{0}]", v_area_id);
			Log::Trace(" ", __FUNCTION__, "v_station_id =[{0}]", v_station_id);
			Log::Trace(" ", __FUNCTION__, "v_func_id =[{0}]", v_func_id);
		}

		
		Log::Trace(" ", __FUNCTION__, "v_func_id =[{0}]", v_func_id);

		c_sql_condition = "select  t.class_code,t.seq_no  "
			" from ted54 t "
			" where t.func_id = @func_id "
			" order by t.seq_no  DESC " //根据序号倒排序，获取最大序号。
			; 
		sqlstr = c_sql_condition;
		cmd_sql.Parameters.Set("func_id", v_func_id); //功能号。
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句  
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			v_class_code = cmd_sql.GetString(1);  
			v_seq_no = cmd_sql.GetDecimal(2); //最大序号。
		}
		cmd_sql.Close();

		if (v_seq_no <= 0)
		{
			sprintf(s.msg, "功能号[%s]还没有维护，无法追加台帐的工序信息。"
				, (const char*)v_func_id);
			throw CApplicationException(-1, s.msg, FunctionEname);
		} 

		Log::Trace("", __FUNCTION__, "IN:c_sql_condition = [{0}] v_seq_no[{1}] "
			, c_sql_condition, v_seq_no);

				
		 v_dev_code = ""; //设备代码
		 v_station_name = ""; //设备名称

	
		 c_sql_condition = " SELECT T.DEV_CODE,T.STATION_NAME "
						   " FROM  TPSSMD1 T "
						   " WHERE T.AREA_ID = @area_id "
						   " AND   T.STATION_ID = NVL(@station_id,STATION_ID)"
						   " ORDER BY T.AREA_ID ,T.STATION_NO ";

		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句  
		cmd_sql.Parameters.Set("area_id", v_area_id);
		cmd_sql.Parameters.Set("station_id", v_station_id);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			
			
			v_dev_code = cmd_sql.GetString(1);
			v_station_name = cmd_sql.GetString(2);

			Log::Trace(" ", __FUNCTION__, "v_dev_code =[{0}]", v_dev_code);
		
			int ii = 0;
	
	  		if (v_wt_name.Trim() != "")
			{
				j = 2;
			}
			else
			{
				j = 1;
			}

			for (int m = 0; m < j; m++)
			{

				//A1_SUM//A1_炉数
				if (m == 0)
				{
					v_item_ename = v_dev_code + "_SUM";
					v_item_cname = v_station_name;
				}
				else
				{
					v_item_ename = v_dev_code + "_WT";
					v_item_cname = v_station_name + v_wt_name;
				}

				//新增校验：若功能号中，该字段已经存在，则继续下一个。
				//============================
				v_cnt = 0;
				c_sql_condition2 = " select count(1) from ted54 t "
					" where t.func_id = @func_id "
					" and   t.item_ename = @item_ename "
					;
				sqlstr = c_sql_condition2;
				cmd_sql2.SetCommandText(c_sql_condition2);// 设置执行的SQL语句  
				cmd_sql2.Parameters.Set("func_id", v_func_id); //功能号。 
				cmd_sql2.Parameters.Set("item_ename", v_item_ename);
				v_cnt = cmd_sql2.ExecuteScalar();
				cmd_sql2.Close();
				if (v_cnt >= 1)
				{
					Log::Trace("", __FUNCTION__, "v_item_ename = [{0}] 已经存在个数[{1}]，不用新增，继续下个。 "
						, v_item_ename, v_cnt);
					continue;
				}


				//序号累计。
				v_seq_no = v_seq_no + 1;

				v_item_must_flag = "0";//非必须。
				v_item_type = "N";
				//v_item_len = "10";

				if (m == 0)
				{
					v_item_len = "10";
				}
				else
				{
					v_item_len = "10,3";
				}


				Log::Trace("", __FUNCTION__, "v_item_ename = [{0}] v_seq_no[{1}] "
					, v_item_ename, v_seq_no);

				//新增表 TED54
				//=============
				c_sql_condition2 = "insert into ted54 "
					"(func_id, class_code, seq_no "
					", item_ename, item_cname "
					", item_must_flag "
					", item_type, item_len) "
					" values(@func_id, @class_code, @seq_no "
					", @item_ename, @item_cname "
					", @item_must_flag "
					", @item_type, @item_len) "
					;

				Log::Trace("", __FUNCTION__, "c_sql_condition2[{0}]  ", c_sql_condition2);
				sqlstr = c_sql_condition2;
				cmd_sql2.SetCommandText(c_sql_condition2);// 设置执行的SQL语句  
				cmd_sql2.Parameters.Set("func_id", v_func_id); //功能号。
				cmd_sql2.Parameters.Set("class_code", v_class_code);
				cmd_sql2.Parameters.Set("seq_no", v_seq_no);

				cmd_sql2.Parameters.Set("item_ename", v_item_ename);
				cmd_sql2.Parameters.Set("item_cname", v_item_cname);
				cmd_sql2.Parameters.Set("item_must_flag", v_item_must_flag);

				cmd_sql2.Parameters.Set("item_type", v_item_type);
				cmd_sql2.Parameters.Set("item_len", v_item_len);
				cmd_sql2.ExecuteNonQuery();
				cmd_sql2.Close();


				//新增表 TED54_RES
				//===============

				v_culture = "zh_Hans"; //简体中文。
				c_sql_condition2 = "insert into ted54_res "
					"(func_id, class_code, culture "
					", item_ename, item_cname "
					"  ) "
					" values(@func_id, @class_code, @culture "
					", @item_ename, @item_cname "
					"  ) "
					;

				Log::Trace("", __FUNCTION__, "c_sql_condition2[{0}]  ", c_sql_condition2);
				sqlstr = c_sql_condition2;
				cmd_sql2.SetCommandText(c_sql_condition2);// 设置执行的SQL语句  
				cmd_sql2.Parameters.Set("func_id", v_func_id); //功能号。
				cmd_sql2.Parameters.Set("class_code", v_class_code);
				cmd_sql2.Parameters.Set("culture", v_culture);

				cmd_sql2.Parameters.Set("item_ename", v_item_ename);
				cmd_sql2.Parameters.Set("item_cname", v_item_cname);
				cmd_sql2.ExecuteNonQuery();
				cmd_sql2.Close();


				
			}
						
			
		}
		cmd_sql.Close();


		c_sql_condition = CString("SELECT CODE,CODE_DESC_1_CONTENT "
			"  FROM  TEP0002  "
			"  WHERE CODE_CLASS = 'M00F' "
			"  AND  CODE_DESC_2_CONTENT = 'SM'"
			"  AND CODE in('1','2','3') ");

		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句  
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{


			v_dev_code = cmd_sql.GetString(1);
			v_station_name = cmd_sql.GetString(2);

			Log::Trace(" ", __FUNCTION__, "v_dev_code =[{0}]", v_dev_code);

			int ii = 0;

			if (v_wt_name.Trim() != "")
			{
				j = 2;
			}
			else
			{
				j = 1;
			}

			for (int m = 0; m < j; m++)
			{

				//A1_SUM//A1_炉数
				if (m == 0)
				{
					v_item_ename = v_dev_code + "_SUM";
					v_item_cname = v_station_name;
				}
				else
				{
					v_item_ename = v_dev_code + "_WT";
					v_item_cname = v_station_name + v_wt_name;
				}

				//新增校验：若功能号中，该字段已经存在，则继续下一个。
				//============================
				v_cnt = 0;
				c_sql_condition2 = " select count(1) from ted54 t "
					" where t.func_id = @func_id "
					" and   t.item_ename = @item_ename "
					;
				sqlstr = c_sql_condition2;
				cmd_sql2.SetCommandText(c_sql_condition2);// 设置执行的SQL语句  
				cmd_sql2.Parameters.Set("func_id", v_func_id); //功能号。 
				cmd_sql2.Parameters.Set("item_ename", v_item_ename);
				v_cnt = cmd_sql2.ExecuteScalar();
				cmd_sql2.Close();
				if (v_cnt >= 1)
				{
					Log::Trace("", __FUNCTION__, "v_item_ename = [{0}] 已经存在个数[{1}]，不用新增，继续下个。 "
						, v_item_ename, v_cnt);
					continue;
				}


				//序号累计。
				v_seq_no = v_seq_no + 1;

				v_item_must_flag = "0";//非必须。
				v_item_type = "N";
				//v_item_len = "10";

				if (m == 0)
				{
					v_item_len = "10";
				}
				else
				{
					v_item_len = "10,3";
				}


				Log::Trace("", __FUNCTION__, "v_item_ename = [{0}] v_seq_no[{1}] "
					, v_item_ename, v_seq_no);

				//新增表 TED54
				//=============
				c_sql_condition2 = "insert into ted54 "
					"(func_id, class_code, seq_no "
					", item_ename, item_cname "
					", item_must_flag "
					", item_type, item_len） "
					" values(@func_id, @class_code, @seq_no "
					", @item_ename, @item_cname "
					", @item_must_flag "
					", @item_type, @item_len) "
					;

				Log::Trace("", __FUNCTION__, "c_sql_condition2[{0}]  ", c_sql_condition2);
				sqlstr = c_sql_condition2;
				cmd_sql2.SetCommandText(c_sql_condition2);// 设置执行的SQL语句  
				cmd_sql2.Parameters.Set("func_id", v_func_id); //功能号。
				cmd_sql2.Parameters.Set("class_code", v_class_code);
				cmd_sql2.Parameters.Set("seq_no", v_seq_no);

				cmd_sql2.Parameters.Set("item_ename", v_item_ename);
				cmd_sql2.Parameters.Set("item_cname", v_item_cname);
				cmd_sql2.Parameters.Set("item_must_flag", v_item_must_flag);

				cmd_sql2.Parameters.Set("item_type", v_item_type);
				cmd_sql2.Parameters.Set("item_len", v_item_len);
				cmd_sql2.ExecuteNonQuery();
				cmd_sql2.Close();


				//新增表 TED54_RES
				//===============

				v_culture = "zh_Hans"; //简体中文。
				c_sql_condition2 = "insert into ted54_res "
					"(func_id, class_code, culture "
					", item_ename, item_cname "
					"  ) "
					" values(@func_id, @class_code, @culture "
					", @item_ename, @item_cname "
					"  ) "
					;

				Log::Trace("", __FUNCTION__, "c_sql_condition2[{0}]  ", c_sql_condition2);
				sqlstr = c_sql_condition2;
				cmd_sql2.SetCommandText(c_sql_condition2);// 设置执行的SQL语句  
				cmd_sql2.Parameters.Set("func_id", v_func_id); //功能号。
				cmd_sql2.Parameters.Set("class_code", v_class_code);
				cmd_sql2.Parameters.Set("culture", v_culture);

				cmd_sql2.Parameters.Set("item_ename", v_item_ename);
				cmd_sql2.Parameters.Set("item_cname", v_item_cname);
				cmd_sql2.ExecuteNonQuery();
				cmd_sql2.Close();



			}


		}
		cmd_sql.Close();

		




		/*设置系统返回参数*/
		strcpy(s.msg, _RES("GCRSS0000002"));//处理成功。  




	}
	/*捕获数据库操作异常*/
	catch(CDbException& ex)  //捕获数据库操作异常
	{  
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

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



	/// <summary>
	/// 返回总记录数
	/// </summary>     
	bcls_ret->Tables.Add("PageInfo");
	bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
	bcls_ret->Tables["PageInfo"].Rows.Add();
	bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = v_total_count;
	 
	 
	////EDLog(1, 1, " **************%s end*****************", (const char*)FunctionEname);

	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;
}