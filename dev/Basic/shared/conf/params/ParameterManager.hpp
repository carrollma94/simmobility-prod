/*
 * ParameterManager.hpp
 *
 *  Created on: Apr 27, 2014
 *      Author: Max
 */

#pragma once

#include <map>
#include <string>

#include "ParamData.hpp"
#include "ParseParamFile.hpp"

namespace sim_mob {

using namespace std;

enum InstanceType
{
  ParameterMgrInstance_Normal = 0,
  ParameterMgrInstance_AMOD = 1
};

class ParameterManager {
public:
	static ParameterManager* Instance(bool isAMOD_InstanceReqested);
	virtual ~ParameterManager();

public:
   /** \brief Assign value from parameter pool, with default.
	*
	* This method tries to retrieve the indicated parameter value from the
	* parameter pool, storing the result in paramVal.  If the value
	* cannot be retrieved from the pool, defaultVal is used instead.
	*
	* \param paramName The key to be searched on the parameter pool.
	* \param[out] paramVal Storage for the retrieved value.
	* \param defaultVal Value to use if the pool doesn't contain this
	* parameter.
	* \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	*/
	template<typename T>
	void param(const std::string& modelName, const std::string& paramName, T& paramVal, const T& defaultVal) const
	{
		if (hasParam(modelName,paramName))
		{
		  if (getParam(modelName,paramName, paramVal))
		  {
			return;
		  }
		}

		paramVal = defaultVal;
	}
	/** \brief Check whether a parameter exists in parameter pool
	*
	* \param key The key to check.
	*
	* \return true if the parameter exists, false otherwise
	* \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	*/
	bool hasParam(const std::string& modelName, const std::string& key) const;

	/** \brief Set an arbitrary XML/RPC value to the parameter pool.
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param v The value to be inserted.
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  void setParam(const std::string& modelName, const std::string& key, const ParamData& v);
	  /** \brief Set a string value on the parameter pool.
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param s The value to be inserted.
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  void setParam(const std::string& modelName, const std::string& key, const std::string& s) ;
	  /** \brief Set a double value on the parameter pool.
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param d The value to be inserted.
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  void setParam(const std::string& modelName, const std::string& key, double d) ;
	  /** \brief Set an integer value on the parameter pool.
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param i The value to be inserted.
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  void setParam(const std::string& modelName, const std::string& key, int i) ;
	  /** \brief Set a boolean value on the parameter pool.
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param b The value to be inserted.
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  void setParam(const std::string& modelName, const std::string& key, bool b) ;
	/** \brief Get a double value from the parameter pool.
	   *
	   * If you want to provide a default value in case the key does not exist use param().
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param[out] d Storage for the retrieved value.
	   *
	   * \return true if the parameter value was retrieved, false otherwise
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  bool getParam(const std::string& modelName, const std::string& key, double& d) const;
	/** \brief Get a string value from the parameter pool
	*
	* \param key The key to be used in the parameter pool's dictionary
	* \param[out] s Storage for the retrieved value.
	*
	* \return true if the parameter value was retrieved, false otherwise
	* \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	*/
	bool getParam(const std::string& modelName, const std::string& key, std::string& s) const;
	/** \brief Get an integer value from the parameter pool.
	   *
	   * If you want to provide a default value in case the key does not exist use param().
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param[out] i Storage for the retrieved value.
	   *
	   * \return true if the parameter value was retrieved, false otherwise
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  bool getParam(const std::string& modelName, const std::string& key, int& i) const;
	  /** \brief Get a boolean value from the parameter pool.
	   *
	   * If you want to provide a default value in case the key does not exist use param().
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param[out] b Storage for the retrieved value.
	   *
	   * \return true if the parameter value was retrieved, false otherwise
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  bool getParam(const std::string& modelName, const std::string& key, bool& b) const;
	  /** \brief Get an arbitrary XML/RPC value from the parameter pool.
	   *
	   * If you want to provide a default value in case the key does not exist use param().
	   *
	   * \param key The key to be used in the parameter pool's dictionary
	   * \param[out] v Storage for the retrieved value.
	   *
	   * \return true if the parameter value was retrieved, false otherwise
	   * \throws InvalidNameException If the parameter key begins with a tilde, or is an otherwise invalid graph resource name
	   */
	  bool getParam(const std::string& modelName, const std::string& key, ParamData& v) const;

private:
	  ParameterManager(bool);
	  typedef std::map< string, ParamData> ParameterNameValueMap;
	  typedef std::map< string, ParamData>::iterator ParameterNameValueMapIterator;
	  typedef std::map< string, ParamData>::const_iterator ParameterNameValueMapConIterator;
	  typedef std::map< string,ParameterNameValueMap >::iterator ParameterPoolIterator;
	  typedef std::map< string,ParameterNameValueMap >::const_iterator ParameterPoolConIterator;
	  /**
	   *  \brief store parameters
	   *  \map key is model name
	   *  \map value is parameters belong to the model
	   *  \sub map key is xml Tag or Element "name"
	   *  \sub map value is XmlRpcValue
	   */
	  std::map< string,ParameterNameValueMap > parameterPool;
	  static std::map<InstanceType, ParameterManager *> instances;
};

}// namespace sim_mob