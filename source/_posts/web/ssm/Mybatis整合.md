---

title: ssm学习02 - jdbc简化?优秀的持久化框架MB整合spirng

tags:    

- "mybatis"
- "数据库"
- "ssm"   
- "maven"

categories:

- "web"
- "ssm架构"

thumbnail: https://s2.loli.net/2024/03/05/Z3VGyjPqgIbXrQ7.png

---

# 1.MyBatis的基础

- Mybatis是一款优秀的**持久层框架**。
- 它支持定制化SQL、存储过程以及高级映射。
- Mybatis避免了几乎所有的JDBC代码和手动设置参数以及获取结果集。
- Mybatis可以使用简单的XML或注解来配置和映射原生类型、接口和Java的POJO(Plain Old Java Object，普通老式Java对象)为数据库中的记录。
- Mybatis本是Apache的一个开源项目Ibatis，2010年这个项目由Apache software foundation迁移到了Google Code，并改名为Mybatis。
- 2013年11月迁移到**Github**。

## (i)核心配置

### Maven依赖

```xml
<!--mysql驱动-->
    <dependency>
      <groupId>mysql</groupId>
      <artifactId>mysql-connector-java</artifactId>
      <version>8.0.21</version>
    </dependency>

  <!--mybatis-->
  <dependency>
      <groupId>org.mybatis</groupId>
      <artifactId>mybatis</artifactId>
      <version>3.5.6</version>
  </dependency>

  <!--junit-->
  <dependency>
    <groupId>junit</groupId>
    <artifactId>junit</artifactId>
    <version>4.13</version>
  </dependency>
```

### mybatis-config.xml

Mybatis的配置文件包含了会深深影响Mybatis行为的设置和属性信息。

> configuration（配置）
> properties（属性）
> settings（设置）
> typeAliases（类型别名）
> typeHandlers（类型处理器）
> objectFactory（对象工厂）
> plugins（插件）
> environments（环境配置）
> environment（环境变量）
> transactionManager（事务管理器）
> dataSource（数据源）
> databaseIdProvider（数据库厂商标识）
> mappers（映射器）

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE configuration
        PUBLIC "-//mybatis.org//DTD Config 3.0//EN"
        "http://mybatis.org/dtd/mybatis-3-config.dtd">
<configuration>


    <!--引入外部配置文件-->
    <properties resource="db.properties"/>

    <typeAliases>
        <typeAlias type="com.mybatis.pojo.User" alias="user"/>
    </typeAliases>


    <environments default="development">

        <environment id="development">
            <transactionManager type="JDBC"/>
            <dataSource type="POOLED">
                <property name="driver" value="${driver}"/>
                <property name="url" value="${url}"/>
                <property name="username" value="${username}"/>
                <property name="password" value="${password}"/>
            </dataSource>
        </environment>


    </environments>

    <mappers>
       <mapper resource="com/mybatis/dao/UserMapper.xml"/>
    </mappers>


</configuration>
```

**db.properties**

绑定数据库连接资源文件（见项目实例）

**映射器**

注册绑定我们的Mapper.xml文件

方式一:

```xml
<!-- 使用相对于类路径的资源引用 -->
<mappers>
  <mapper resource="org/mybatis/builder/AuthorMapper.xml"/>
</mappers>
```

方式二:

```xml
<!-- 使用映射器接口实现类的完全限定类名 -->
<mappers>
  <mapper class="org.mybatis.builder.AuthorMapper"/>
</mappers>
```

## (ii)Mapper.xml

### 常见格式

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE mapper
       PUBLIC "-//mybatis.org//DTD Mapper 3.0//EN"
       "http://mybatis.org/dtd/mybatis-3-mapper.dtd">

<mapper namespace="com.mybatis.dao.UserMapper">
  <select id="getUserList" resultType="com.mybatis.pojo.User">
        select * from mybatis.user
    </select>
</mapper>
```

在接口中声明方法名，在Mapper.xml中用select,insert,update,delete组件对该方法名定义，在测试类中使用该方法

### 增删改查

**select**

- id : 就是对应的namespace中的方法名；
- resultType：Sql语句执行的返回值;
- parameterType ： 参数类型;

1.编写接口

```java
//根据ID查询用户
   User getUserById(int id);
```

2.编写对应的mapper中的sql语句

```xml
   <select id="getUserById" parameterType="int" resultType="com.kuang.pojo.User">
           select * from mybatis.user where id = #{id}
   </select>
  
```

3.测试

```java
       @Test
       public void getUserById() {
           SqlSession sqlSession = MybatisUtils.getSqlSession();
           UserMapper mapper = sqlSession.getMapper(UserMapper.class);
           User user = mapper.getUserById(1);
           System.out.println(user);
           sqlSession.close();
       }
```

**Insert**

```xml
<!--对象中的属性，可以直接取出来-->
    <insert id="addUser" parameterType="com.kuang.pojo.User">
        insert into mybatis.user (id, name, pwd) values (#{id},#{name},#{pwd});
    </insert>
```

**update**

```xml
    <update id="updateUser" parameterType="com.kuang.pojo.User">
        update mybatis.user set name=#{name},pwd=#{pwd}  where id = #{id} ;
    </update>
```

**delete**

```xml
  <delete id="deleteUser" parameterType="int">
        delete from mybatis.user where id = #{id};
  </delete>
```

**注意点**

- 增删改需要提交事务！
- 手动提交

```text
    sqlSession.commit();
```

- 自动提交

![v2-16650aebd5faa09a9ac2298467a979b8_1440w](https://s2.loli.net/2024/03/05/9gFiXvJauhxKtHr.webp)

### 多参传递

参数过多时可以使用Map<String,Object>进行传递

```java
	int addUser(Map<String,Object> map);

    <!--对象中的属性，可以直接取出来    传递map的key-->
    <insert id="addUser" parameterType="map">
        insert into mybatis.user (id, pwd) values (#{userid},#{passWord});
    </insert>

    @Test
    public void addUser(){
        SqlSession sqlSession = MybatisUtils.getSqlSession();
        UserMapper mapper = sqlSession.getMapper(UserMapper.class);
        Map<String, Object> map = new HashMap<String, Object>();
        map.put("userid",5);
        map.put("passWord","2222333");
        mapper.addUser(map);
        sqlSession.close();
    }
```

### 属性名和字段名不一致

- 解决方法1：起别名

  ```xml
  select * from muser
  select id,name,pwd from muser where id = #{id}
  ```

- 解决方法2：ResultMap

  **id属性** ，resultMap标签的标识。

  **type属性** ，返回值的全限定类名，或类型别名。

  **autoMapping属性** ，值范围true（默认值）|false, 设置是否启动自动映射功能，自动映射功能就是自动查找与字段名小写同名的属性名，并调用setter方法。而设置为false后，则需要在`resultMap`内明确注明映射关系才会调用对应的setter方法。

  ```xml
  <!--结果集映射-->
     <resultMap id="UserMap" type="User">
      <!--column数据库中的字段，property实体类中的属性-->
          <result column="id" property="id"/>
          <result column="name" property="name"/>
          <result column="pwd" property="password"/>
      </resultMap>
      
    <select id="getUserByid" resultMap="UserMap">
       select * from mybatis.user where id =#{id}
    <select>
  ```

## (iii)项目实例

db.properties

```properties
driver=com.mysql.cj.jdbc.Driver
url=jdbc:mysql://localhost:3306/mybatis?serverTimezone=UTC&useSSL=true&useUnicode=true&characterEncoding=UTF-8
username=root
password=123456
```

mybatis工具类

```java
//sqlSessionFactory -->SqlSession
public class MybatisUtils {

    private static SqlSessionFactory sqlSessionFactory;

    static {
        try {
            //使用Mybatis第一步: 获取sqlSessionFactory对象
            String resource = "mybatis-config.xml";
            InputStream inputStream = Resources.getResourceAsStream(resource);
            sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    //既然有了 SqlSessionFactory，顾名思义，我们可以从中获得 SqlSession 的实例。
    //sqlSession 完全包含了面向数据库执行SQL命令所需的所有方法
    public static SqlSession getSqlseesion() {
        // SqlSession sqlSession = sqlSessionFactory.openSession();
        return sqlSessionFactory.openSession();
    }
}
```

mybatis-config.xml

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE configuration
        PUBLIC "-//mybatis.org//DTD Config 3.0//EN"
        "http://mybatis.org/dtd/mybatis-3-config.dtd">
<configuration>
    <!--引入外部配置文件-->
    <properties resource="db.properties"/>
    <typeAliases>
        <typeAlias type="com.mybatis.pojo.User" alias="user"/>
    </typeAliases>

    <environments default="development">

        <environment id="development">
            <transactionManager type="JDBC"/>
            <dataSource type="POOLED">
                <property name="driver" value="${driver}"/>
                <property name="url" value="${url}"/>
                <property name="username" value="${username}"/>
                <property name="password" value="${password}"/>
            </dataSource>
        </environment>

    </environments>

    <mappers>
       <mapper resource="com/mybatis/dao/UserMapper.xml"/>
    </mappers>

</configuration>
```

实体类User

```java
public class User {
   private int id;  //id
   private String name;   //姓名
   private String pwd;   //密码
   public int getId() {
      return id;
   }
   public void setId(int id) {
       this.id = id;
   }
   public String getName() {
       return name;
   }
   public void setName(String name) {
       this.name = name;
   }
   public String getPwd() {
       return pwd;
   }
   public void setPwd(String pwd) {
       this.pwd = pwd;
   }
   @Override
   public String toString() {
       return "User{" +
               "id=" + id +
               ", name='" + name + '\'' +
               ", pwd='" + pwd + '\'' +
               '}';
   }  
}
```

Mapper接口

```java
import com.mybatis.pojo.User;
import java.util.List;
public interface UserMapper {
   //查询全部用户
   List<User> getUserList();
}
```

Mapper.xml

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE mapper
       PUBLIC "-//mybatis.org//DTD Mapper 3.0//EN"
       "http://mybatis.org/dtd/mybatis-3-mapper.dtd">

<mapper namespace="com.mybatis.dao.UserMapper">
  <select id="getUserList" resultType="com.mybatis.pojo.User">
        select * from mybatis.user
    </select>
</mapper>
```

测试

```java
 @Test
    public void test(){

        SqlSession sqlSession = MybatisUtils.getSqlseesion();

        UserDao userdao = sqlSession.getMapper(UserDao.class);
        List<User> userList = userdao.getUserList();

        for(User user : userList){
            System.out.println(user);
        }

        sqlSession.close();

    }
```

# 2.Spring项目与Mybatis的整合

## ☆项目配置

```xml
<dependencies>
<dependency>
    <groupId>junit</groupId>
    <artifactId>junit</artifactId>
    <version>4.12</version>
</dependency>
<dependency>
    <groupId>org.mybatis</groupId>
    <artifactId>mybatis</artifactId>
    <version>3.5.2</version>
</dependency>
<dependency>
    <groupId>mysql</groupId>
    <artifactId>mysql-connector-java</artifactId>
    <version>5.1.47</version>
</dependency>
<dependency>
    <groupId>org.springframework</groupId>
    <artifactId>spring-webmvc</artifactId>
    <version>5.1.10.RELEASE</version>
</dependency>
<dependency>
    <groupId>org.springframework</groupId>
    <artifactId>spring-jdbc</artifactId>
    <version>5.1.10.RELEASE</version>
</dependency>
<!-- https://mvnrepository.com/artifact/org.aspectj/aspectjweaver -->
<dependency>
    <groupId>org.aspectj</groupId>
    <artifactId>aspectjweaver</artifactId>
    <version>1.9.4</version>
</dependency>
<dependency>
    <groupId>org.mybatis</groupId>
    <artifactId>mybatis-spring</artifactId>
    <version>2.0.2</version>
</dependency>
</dependencies>
<!--配置Maven静态资源过滤问题！-->
<build>
    <resources>
        <resource>
            <directory>src/main/java</directory>
            <includes>
                <include>**/*.properties</include>
                <include>**/*.xml</include>
            </includes>
            <filtering>true</filtering>
        </resource>
    </resources>
</build>
```

## 整合实现

### 实现方法一

**bean.xml**

头内容

```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://www.springframework.org/schema/beans
        http://www.springframework.org/schema/beans/spring-beans.xsd">
```

配置数据源替换mybaits的数据源

```xml
<!--配置数据源：数据源有非常多，可以使用第三方的，也可使使用Spring的-->
<bean id="dataSource" class="org.springframework.jdbc.datasource.DriverManagerDataSource">
    <property name="driverClassName" value="com.mysql.jdbc.Driver"/>
    <property name="url" value="jdbc:mysql://localhost:3306/mybatis?useSSL=true&amp;useUnicode=true&amp;characterEncoding=utf8"/>
    <property name="username" value="root"/>
    <property name="password" value="123456"/>
</bean>
```

配置SqlSessionFactory，关联MyBatis

```xml
<!--配置SqlSessionFactory-->
<bean id="sqlSessionFactory" class="org.mybatis.spring.SqlSessionFactoryBean">
    <property name="dataSource" ref="dataSource"/>
    <!--关联Mybatis-->
    <property name="configLocation" value="classpath:mybatis-config.xml"/>
    <property name="mapperLocations" value="classpath:com/kuang/dao/*.xml"/>
</bean>
```

注册sqlSessionTemplate，关联sqlSessionFactory

```xml
<!--注册sqlSessionTemplate , 关联sqlSessionFactory-->
<bean id="sqlSession" class="org.mybatis.spring.SqlSessionTemplate">
    <!--利用构造器注入-->
    <constructor-arg index="0" ref="sqlSessionFactory"/>
</bean>
```

注册bean实现

```xml
<bean id="userDao" class="com.kuang.dao.UserDaoImpl">
    <property name="sqlSession" ref="sqlSession"/>
</bean>
```

**java**

增加Dao接口的实现类；私有化sqlSessionTemplate

```java
public class UserDaoImpl implements UserMapper {
    //sqlSession不用我们自己创建了，Spring来管理
    private SqlSessionTemplate sqlSession;
    public void setSqlSession(SqlSessionTemplate sqlSession) {
        this.sqlSession = sqlSession;
    }
    public List<User> selectUser() {
        UserMapper mapper = sqlSession.getMapper(UserMapper.class);
        return mapper.selectUser();
    }
}
```

测试

```java
@Test
public void test2(){
    ApplicationContext context = new ClassPathXmlApplicationContext("beans.xml");
    UserMapper mapper = (UserMapper) context.getBean("userDao");
    List<User> user = mapper.selectUser();
    System.out.println(user);
}
```

**maybatis-config.xml**

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE configuration
        PUBLIC "-//mybatis.org//DTD Config 3.0//EN"
        "http://mybatis.org/dtd/mybatis-3-config.dtd">
<configuration>
    <typeAliases>
        <package name="com.kuang.pojo"/>
    </typeAliases>
</configuration>
```

### 实现方法二

dao继承Support类 , 直接利用 getSqlSession() 获得 , 然后直接注入SqlSessionFactory . 比起方式1 , 不需要管理SqlSessionTemplate , 而且对事务的支持更加友好 . 可跟踪源码查看

![kuangstudyb25452cf-79e0-4c0c-a93e-680f2df26509](https://s2.loli.net/2024/03/05/2VaOIAiylsDPQe4.png)

1.将我们方法1写的UserDaoImpl修改一下

```java
public class UserDaoImpl extends SqlSessionDaoSupport implements UserMapper {
    public List<User> selectUser() {
        UserMapper mapper = getSqlSession().getMapper(UserMapper.class);
        return mapper.selectUser();
    }
}
```

2.修改bean的配置

```xml
<bean id="userDao" class="com.kuang.dao.UserDaoImpl">
    <property name="sqlSessionFactory" ref="sqlSessionFactory" />
</bean>
```

# 3.事务提交

事务就是把一系列的动作当成一个独立的工作单元，这些动作要么全部完成，要么全部不起作用

**事务四个属性ACID**

1. 原子性（atomicity）
   - 事务是原子性操作，由一系列动作组成，事务的原子性确保动作要么全部完成，要么完全不起作用
2. 一致性（consistency）
   - 一旦所有事务动作完成，事务就要被提交。数据和资源处于一种满足业务规则的一致性状态中
3. 隔离性（isolation）
   - 可能多个事务会同时处理相同的数据，因此每个事务都应该与其他事务隔离开来，防止数据损坏
4. 持久性（durability）
   - 事务一旦完成，无论系统发生什么错误，结果都不会受到影响。通常情况下，事务的结果被写到持久化存储器中

**事务管理方式**

1.编程式事务管理

- 将事务管理代码嵌到业务方法中来控制事务的提交和回滚
- 缺点：必须在每个事务操作业务逻辑中包含额外的事务管理代码

2.声明式事务管理

- 一般情况下比编程式事务好用。
- 将事务管理代码从业务方法中分离出来，以声明的方式来实现事务管理。
- 将事务管理作为横切关注点，通过aop方法模块化。

**使用Spring管理事务，注意头文件的约束导入 : tx**

```xml
mlns:tx="http://www.springframework.org/schema/tx"
http://www.springframework.org/schema/tx
http://www.springframework.org/schema/tx/spring-tx.xsd">
```

**事务管理器**

```xml
<bean id="transactionManager" class="org.springframework.jdbc.datasource.DataSourceTransactionManager">
        <property name="dataSource" ref="dataSource" />
 </bean>
```

**配置好事务管理器后我们需要去配置事务的通知**

```xml
<!--配置事务通知-->
<tx:advice id="txAdvice" transaction-manager="transactionManager">
    <tx:attributes>
        <!--配置哪些方法使用什么样的事务,配置事务的传播特性-->
        <tx:method name="add" propagation="REQUIRED"/>
        <tx:method name="delete" propagation="REQUIRED"/>
        <tx:method name="update" propagation="REQUIRED"/>
        <tx:method name="search*" propagation="REQUIRED"/>
        <tx:method name="get" read-only="true"/>
        <tx:method name="*" propagation="REQUIRED"/>
    </tx:attributes>
</tx:advice>
```

**spring事务传播特性：**

事务传播行为就是多个事务方法相互调用时，事务如何在这些方法间传播。spring支持7种事务传播行为：

- propagation_requierd：如果当前没有事务，就新建一个事务，如果已存在一个事务中，加入到这个事务中，这是最常见的选择。
- propagation_supports：支持当前事务，如果没有当前事务，就以非事务方法执行。
- propagation_mandatory：使用当前事务，如果没有当前事务，就抛出异常。
- propagation_required_new：新建事务，如果当前存在事务，把当前事务挂起。
- propagation_not_supported：以非事务方式执行操作，如果当前存在事务，就把当前事务挂起。
- propagation_never：以非事务方式执行操作，如果当前事务存在则抛出异常。
- propagation_nested：如果当前存在事务，则在嵌套事务内执行。如果当前没有事务，则执行与propagation_required类似的操作

**配置AOP**

导入aop的头文件！

```xml
<!--配置aop织入事务-->
<aop:config>
    <aop:pointcut id="txPointcut" expression="execution(* com.kuang.dao.*.*(..))"/>
    <aop:advisor advice-ref="txAdvice" pointcut-ref="txPointcut"/>
</aop:config>
```

**为什么需要配置事务？**

- 如果不配置，就需要我们手动提交控制事务；
- 事务在项目开发过程非常重要，涉及到数据的一致性的问题，不容马虎！
