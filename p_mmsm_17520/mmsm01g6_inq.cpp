/*<remark>=========================================================
/// <summary>
/// 开浇信息查询
/// <para>
/// 1.根据浇注结束时刻,厂别分区进行开浇信息查询。
/// </para>
/// <para>数据库表：
///1.炼钢连铸作业实绩表 TMMSM31
///</para>
/// <para>主调用函数：        </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns>开浇信息查询 </returns>
===========================================================</remark>*/

#include "stdafx.h"

BM2F_ENTERACE(mmsm01g6_inq)


int f_mmsm01g6_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		CString v_prod_time_f = "", v_prod_time_e = "";
		CString v_div_st = "",v_area_id="",v_station_id=""; 
		CString tempsql = "";
		CString sql_where = ""; CString sql_castsum_where = "";
		
		CDbCommand com(conn); CDbCommand com_castsum(conn);

		CDecimal TotalRecord;
		int  v_factory_sum=0,v_dev_code_sum=0;
		CPageInfo pageInfo;
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 500;
		}


		CString v_factory[100], v_dev_code[100], v_wt_name;//统计炼钢广别和机器种类上限为100个

		v_prod_time_f = bcls_rec->Tables[0].Rows[0]["prod_time_f"].ToString().Trim();
		v_prod_time_e = bcls_rec->Tables[0].Rows[0]["prod_time_e"].ToString().Trim();
		v_div_st = bcls_rec->Tables[0].Rows[0]["div_st"].ToString().Trim();
		v_area_id = bcls_rec->Tables[0].Rows[0]["area_id"].ToString().Trim();
		v_station_id = bcls_rec->Tables[0].Rows[0]["station_id"].ToString().Trim();
		v_wt_name = bcls_rec->Tables[0].Rows[0]["wt_name"].ToString().Trim();

		Log::Info("", __FUNCTION__, "prod_time_f=[{0}];prod_time_e=[{1}];v_div_st=[{2}];", v_prod_time_f, v_prod_time_e, v_div_st);
		Log::Info("", __FUNCTION__, "分页信息：PageSize=[{0}];RecordFrom=[{1}]", pageInfo.PageSize, pageInfo.RecordFrom);

		if (v_area_id.GetLength() <= 0 ){
			sprintf(s.msg, "后台未获取到炼钢区域标识（area_id=[{0}]）)，"
				"不能自动设置统计项的数据",(const char *)v_area_id);
			throw CApplicationException(-1, s.sysmsg, "mmsm01g6_inq");
		}
		Log::Info("", __FUNCTION__, "区域标识和设备类型信息：AREA_ID=[{0}];STATION_ID=[{1}]", v_area_id, v_station_id);
		if (v_prod_time_f.GetLength() > 0){
			if (v_prod_time_f.GetLength() == 8){
				v_prod_time_f += "000000";
				sql_where += " AND A.POUR_END_TIME >= @v_prod_time_f ";
				sql_castsum_where += " AND A.POUR_END_TIME >= @v_prod_time_f ";
			}
			else if (v_prod_time_f.GetLength() == 14){
				sql_where += " AND A.POUR_END_TIME >= @v_prod_time_f ";
				sql_castsum_where += " AND A.POUR_END_TIME >= @v_prod_time_f ";
			}
			else
			{
				sprintf(s.msg, "获取起始时间为[%s],不是14或者8位", (const char*)v_prod_time_f);
				throw CApplicationException(-1, s.msg, "mmsm01g6_inq");
			}
			com.Parameters.Set("v_prod_time_f", v_prod_time_f);
			/*com_castsum.Parameters.Set("v_prod_time_f", v_prod_time_f);*/
		}
		if (v_prod_time_e.GetLength() > 0){
			if (v_prod_time_e.GetLength() == 8){
				v_prod_time_e += "235959";
				sql_where += " AND A.POUR_END_TIME<= @v_prod_time_e ";
				sql_castsum_where += " AND A.POUR_END_TIME<= @v_prod_time_e ";
			}
			else if (v_prod_time_e.GetLength() == 14){
				sql_where += " AND A.POUR_END_TIME <= @v_prod_time_e ";
				sql_castsum_where += " AND A.POUR_END_TIME<= @v_prod_time_e ";
			}
			else
			{
				sprintf(s.msg, "获取起始时间为[%s],不是14或者8位", (const char*)v_prod_time_e);
				throw CApplicationException(-1, s.msg, "mmsm01g6_inq");
			}
			com.Parameters.Set("v_prod_time_e", v_prod_time_e);
		/*	com_castsum.Parameters.Set("v_prod_time_e", v_prod_time_e);*/
		}
		if (v_div_st.GetLength() > 0){
			sql_where += " AND A.FACTORY_DIV LIKE @v_div_st ";
			com.Parameters.Set("v_div_st", v_div_st);
		}
		tempsql = " SELECT DISTINCT PONO, HEAT_NO, PREC_ST_NO, POUR_START_TIME, POUR_END_TIME, CAST_NO, CAST_DIV_NO, "
			" TD_NO_1||(CASE WHEN TD_NO_1>'' AND TD_NO_2>'' THEN '-' ELSE '' END)||TD_NO_2 AS TD_NO,SLAB_DEST,SLAB_WT "
			" FROM TMMSM31 A WHERE 1=1 ";

		///计算显示项查询结果的记录数
		Log::Info("", "", "计算显示项查询结果的记录数位置");
		sqlstr = "SELECT COUNT(*) FROM( "+tempsql + sql_where+")";
		Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		com.SetCommandText(sqlstr);
		TotalRecord = com.ExecuteScalar();
		com.Close();
		if (bcls_ret->Tables.Contains("PageInfo")){
			bcls_ret->Tables.Remove("PageInfo");
		}
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecord;

		///获取和设置显示项的查询结果
		///①
		sqlstr = " SELECT A.*, (CASE WHEN A.CAST_NO!='' THEN COUNT(B.CAST_NO) ELSE NULL END ) AS CAST_PONO_SUM "
			" FROM (" + tempsql + sql_where + ")AS A LEFT JOIN ("
			"   SELECT CAST_NO "
			"   FROM TMMSM31 A WHERE 1 = 1  " + sql_castsum_where +
			" ) AS B ON A.CAST_NO = B.CAST_NO "
			" group by A.CAST_NO, A.PONO, A.HEAT_NO, A.PREC_ST_NO, A.POUR_START_TIME, A.POUR_END_TIME, A.CAST_NO, A.CAST_DIV_NO,"
			" A.TD_NO, A.SLAB_DEST, A.SLAB_WT "
			" ORDER  BY  A.HEAT_NO ";
		com.SetCommandText(sqlstr);
		com.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		com.Close();
		///或②
		//sqlstr = tempsql +sql_where +" ORDER  BY HEAT_NO";
		//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		//com.SetCommandText(sqlstr);
		//int count=com.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom,  pageInfo.PageSize);
		//com.Close();
		/////额外添加显示项的连连浇浇灌数列，  待优化（先搜集再赋值)
		//Log::Info("", "", "连连浇浇灌数列位置");
		//bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "CAST_PONO_SUM");
		//sqlstr = "SELECT COUNT(*) FROM (" + tempsql + sql_castsum_where + ") AS A WHERE A.CAST_NO LIKE @v_cast_no ";
		//Log::Info("", __FUNCTION__, "count=[{0}]", count);
		//for (int i = 0; i < count; i++){
		//	CString cast = bcls_ret->Tables[0].Rows[i]["CAST_NO"].ToString().Trim();
		//	if (cast.GetLength()>0){
		//		com_castsum.SetCommandText(sqlstr);
		//		/*if (com_castsum.Parameters.Contains("v_prod_time_f")){
		//			Log::Info("", __FUNCTION__, "参数v_prod_time_f=[{0}]",
		//				com_castsum.Parameters.GetString("v_prod_time_f"));
		//		}
		//		if (com_castsum.Parameters.Contains("v_prod_time_e")){
		//			Log::Info("", __FUNCTION__, "参数v_prod_time_e=[{0}]",
		//				 com_castsum.Parameters.GetString("v_prod_time_e"));
		//		}*/
		//		com_castsum.Parameters.Set("v_cast_no", cast);
		//		com_castsum.SetCommandText(sqlstr);
		//		CDecimal cnt = com_castsum.ExecuteScalar();;
		//		bcls_ret->Tables[0].Rows[i]["CAST_PONO_SUM"] =cnt;
		//		com_castsum.Close();
		//		
		//	}
		//}

		////计算统计项

	    //获取炼钢厂别
		Log::Info("", "", "获取炼钢厂别位置");
		sqlstr = "SELECT CODE FROM TEP0002 WHERE CODE_CLASS LIKE 'M00F' "
			" AND CODE_DESC_2_CONTENT LIKE 'SM' ";
		com.SetCommandText(sqlstr);
		com.ExecuteReader();
		while (com.Read()){
			v_factory[v_factory_sum] = com.GetString(1);
			Log::Info("", "", "炼钢厂别factory_div=[{0}]",v_factory[v_factory_sum]);
			v_factory_sum++;
		}
		com.Close();
		//获取设备代码
		Log::Info("", "", "获取设备代码位置");
		com.Parameters.Clear();
		sqlstr = " SELECT DEV_CODE FROM TPSSMD1 WHERE AREA_ID LIKE @v_area_id ";
		if (v_station_id.GetLength() > 0){
			sqlstr += " AND STATION_ID LIKE @v_station_id "; 
			com.Parameters.Set("v_station_id", v_station_id);
		}
		com.SetCommandText(sqlstr);
		com.Parameters.Set("v_area_id", v_area_id);
		
		Log::Info("", __FUNCTION__, "参数v_area_id=[{0}]",
				com.Parameters.GetString("v_area_id"));
		if (com.Parameters.Contains("v_station_id")){
			Log::Info("", __FUNCTION__, "参数v_station_id=[{0}]",
				com.Parameters.GetString("v_station_id"));
		}
		com.ExecuteReader();
		Log::Info("", "", "开始读取设备代码");
		while (com.Read())
		{
			Log::Info("", "", "开始读取第{0}个设备代码", v_dev_code_sum + 1);
			v_dev_code[v_dev_code_sum] = com.GetString(1);
			Log::Info("", "", "设备代码dev_code=[{0}]", v_dev_code[v_dev_code_sum]);
			v_dev_code_sum++;
		}
		com.Close();
		Log::Info("", __FUNCTION__, "v_factory_sum=[{0}];v_dev_code=[{1}]", v_factory_sum, v_dev_code_sum);

		bool flag = false;//判断是否把WT这一列加入结果
		if (v_wt_name.GetLength() > 0){
			flag = true;
		}

		if (bcls_ret->Tables.Contains("MMSMTJ")){
			bcls_ret->Tables.Remove("MMSMTJ");
		}
		bcls_ret->Tables.Add("MMSMTJ");
		for (int i = 0; i < v_factory_sum; i++){
			CString sum_item;
			for (int j = 0; j < v_dev_code_sum; j++){
				sum_item  = v_factory[i] + "_" + v_dev_code[j] + "_SUM";
				bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_DECIMAL,sum_item.Trim());
				if(flag){
					sum_item = v_factory[i] + "_" + v_dev_code[j] + "_WT";
					bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_DECIMAL, sum_item.Trim());
				}
			}
			//此处添加统计项厂别的总统计
			sum_item = v_factory[i] + "_SUM";
			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_DECIMAL, sum_item.Trim());
			if (flag){
				sum_item = v_factory[i]  + "_WT";
				bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_DECIMAL, sum_item.Trim());
			}
			
		}
		Log::Info("", "", "已经添加[{0}]列到表MMSMTJ中", bcls_ret->Tables["MMSMTJ"].Columns.get_Count());
		bcls_ret->Tables["MMSMTJ"].Rows.Add();
		///生产日期时间
		if (flag){
			sqlstr = "SELECT COUNT(*),NVL(SUM(SLAB_WT),0) FROM TMMSM31 A WHERE STATION_ID||STATION_NO LIKE @v_dev_code"
				" AND FACTORY_DIV LIKE @v_factory  "+sql_castsum_where;
		}
		else{
			sqlstr = "SELECT COUNT(*)  FROM TMMSM31 A WHERE STATION_ID||STATION_NO LIKE @v_dev_code"
				" AND FACTORY_DIV LIKE @v_factory " + sql_castsum_where;
		}
		com.Parameters.Clear();
		com.SetCommandText(sqlstr);
		if (v_prod_time_f.GetLength() > 0){
			com.Parameters.Set("v_prod_time_f", v_prod_time_f);
		}
		if (v_prod_time_e.GetLength() > 0){
			com.Parameters.Set("v_prod_time_e", v_prod_time_e);
		}
		for (int i = 0; i < v_factory_sum; i++)
		{
			for (int j = 0; j < v_dev_code_sum; j++)
			{
				Log::Info("", "", "v_dev_code=[{0}]v_factory=[{1}]", v_dev_code[j], v_factory[i]);
				Log::Info("", "", "sqlstr=[{0}]", sqlstr);
				com.Parameters.Set("v_dev_code", v_dev_code[j]);
				com.Parameters.Set("v_factory", v_factory[i]);
				com.ExecuteReader();
				while (com.Read())
				{
					CString sum_item = v_factory[i] + "_" + v_dev_code[j] + "_SUM";
					int index=bcls_ret->Tables["MMSMTJ"].Columns.IndexOf(sum_item.Trim());
					Log::Info("", "", "sum_index=[{0}]", index);
					if (index >= 0){
						bcls_ret->Tables["MMSMTJ"].Rows[0][index] = com.GetDecimal(1);
					}
					if (flag){
						sum_item = v_factory[i] + "_" + v_dev_code[j] + "_WT";
						int index = bcls_ret->Tables["MMSMTJ"].Columns.IndexOf(sum_item.Trim());
						Log::Info("", "", "wt_index=[{0}]", index);
						if (index >= 0){
							bcls_ret->Tables["MMSMTJ"].Rows[0][index] = com.GetDecimal(2);
							Log::Info("", "", "Row[0][{0}]=[{1}]", sum_item, com.GetDecimal(2));
						}
					}
					break;
				}
				com.Close(); 
			}
		}
		//计算统计项的总广别的炉数(和板坯重量)
		com.Parameters.Clear();
		if (flag){
			sqlstr = "SELECT COUNT(*),NVL(SUM(SLAB_WT),0) FROM TMMSM31 A WHERE  FACTORY_DIV LIKE @v_factory  " + sql_castsum_where;
		}
		else{
			sqlstr = "SELECT COUNT(*)  FROM TMMSM31 A WHERE  FACTORY_DIV LIKE @v_factory " + sql_castsum_where;
		}
		com.SetCommandText(sqlstr);
		if (v_prod_time_f.GetLength() > 0){
			com.Parameters.Set("v_prod_time_f", v_prod_time_f);
		}
		if (v_prod_time_e.GetLength() > 0){
			com.Parameters.Set("v_prod_time_e", v_prod_time_e);
		}
		for (int i = 0; i < v_factory_sum; i++){
			com.Parameters.Set("v_factory", v_factory[i]);
			Log::Info("", "", "[{0}]厂别的统计", v_factory[i]);
			com.ExecuteReader();
			while (com.Read())
			{
				CString sum_item = v_factory[i] + "_SUM";
				int index = bcls_ret->Tables["MMSMTJ"].Columns.IndexOf(sum_item.Trim());
				Log::Info("", "", "sum_index=[{0}]", index);
				if (index >= 0){
					bcls_ret->Tables["MMSMTJ"].Rows[0][index] = com.GetDecimal(1);
				}
				if (flag){
					sum_item = v_factory[i]  + "_WT";
					int index = bcls_ret->Tables["MMSMTJ"].Columns.IndexOf(sum_item.Trim());
					Log::Info("", "", "wt_index=[{0}]", index);
					if (index >= 0){
						bcls_ret->Tables["MMSMTJ"].Rows[0][index] = com.GetDecimal(2);
						Log::Info("", "", "Row[0][{0}]=[{1}]", sum_item, com.GetDecimal(2));
					}
				}
				break;
			}
			com.Close();

		 
		}


	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error，sqlcode=[{0},{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		Log::Info("", __FUNCTION__, "erro=[{0}];", s.sysmsg);
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


