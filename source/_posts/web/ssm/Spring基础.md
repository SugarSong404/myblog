---

title: ssm学习01 - 一文搞懂spirng基础

tags:    

- "spring"
- "ssm"   
- "maven"

categories:

- "web"
- "ssm架构"

thumbnail: https://s2.loli.net/2024/03/05/Z3VGyjPqgIbXrQ7.png

---

# 1.如何理解IOC(控制反转)

IoC（Inverse of Control:控制反转）是一种**设计思想** 或者说是某种模式。这个设计思想就是 **将原本在程序中手动创建对象的控制权，交由 Spring 框架来管理。** IoC 在其他语言中也有应用，并非 Spring 特有。**IoC 容器是 Spring 用来实现 IoC 的载体， IoC 容器实际上就是个 Map（key，value）,Map 中存放的是各种对象。

好处呢

1. 对象之间的耦合度或者说依赖程度降低；
2. 资源变的容易管理；比如你用 Spring 容器提供的话很容易就可以实现一个单例。

**举个现实生活的例子。**

假设你希望在不同互联网平台上展示一致的头像，例如个人网站、博客、内容平台、社交平台和招聘平台，等等。对创作者而言这是个常见需求，因为一致的形象有助于提高个人辨识度，从而增加影响力。

如果你维护着10个平台的账号，那么每次更换头像都要去10个账号上重复上传同一张图像，显然不太方便。

于是有了Gravatar这样的“头像配置中心”。你可以在 Gravatar 配置自己的头像，然后在支持 Gravatar 的互联网平台上关联自己的 Gravatar 账号。这样一来，只要更新了 Gravatar 头像，那么所有那些关联了 Gravatar 账号的互联网平台都会自动更新头像。

换言之，你在互联网平台为自己账号设置的不是头像文件本身，而是“如何获取我的最新头像”的方法：调用 Gravatar 的 API。

**控制反转（IoC）也是同样的思想，只不过是在组件层面**。

例如，你写了一个组件 A，负责计算一件商品中所包含的税款。很多组件都用到了组件 A，或者说它们都“依赖”组件 A。

有一天，计算税款的方法变了。也许是政策调整、也许是公司不再适用于之前的纳税标准，总之在之前用到组件 A 的地方，现在需要用另一套方法来计算税款。

于是你新写了一个组件 B，用来按照新标准计算税款。

（等一下，为什么不直接修改组件 A，把它改成最新的税款计算逻辑？很多情况下这是行不通的，例如组件 A 所代表的旧的税款计算方式仍在一些特定场景中用到，所以不能直接改动。即便现在不会用到组件 A，也不能保证将来不会再用到它，总之我们希望保持组件 A 不变。）

这时候，问题来了：所有依赖组件 A 的地方，现在都要把它替换成组件 B！

且不说这样改容易遗漏，即便不出问题，也不是一个好习惯。如果每次改动底层逻辑，都要修改所有依赖它的组件，这样的依赖关系显然是脆弱和僵硬的。

这时候，控制反转（IoC）思想就派上用场了。

正确的做法是，在一开始设计组件依赖的时候就考虑到未来出现改动的可能性。所有依赖组件 A 的组件，都不直接 import 组件 A，而是 import 一个接口 I。它们会说，“给我一个实现了接口 I 的组件！”在 Spring 中，这就是 Autowired 注解（或者等价的 Resource 注解）的含义。

组件 A 实现了接口 I，组件 B 也同样会实现接口 I。如果之后希望把某个场景下所有用到组件 A 的地方都替换为组件 B，那么只需要在相应场景的 Spring 配置中把组件 A 改为组件 B 即可。

换言之，任何业务组件都不再直接控制import哪个具体组件，而是把这个控制权交给 Spring 的配置中心。

这就像是，并不直接上传头像，而是把决定头像的控制权交给 Gravatar 这样的“头像配置中心”。

# 2.如何理解AOP(面向切面编程)

要理解切面编程，就需要先理解什么是切面。用刀把一个西瓜分成两瓣，切开的切口就是切面；炒菜，锅与炉子共同来完成炒菜，锅与炉子就是切面。web层级设计中，web层->网关层->服务层->数据层，每一层之间也是一个切面。编程中，对象与对象之间，方法与方法之间，模块与模块之间都是一个个切面。

我们一般做活动的时候，一般对每一个接口都会做活动的有效性校验（是否开始、是否结束等等）、以及这个接口是不是需要用户登录。

**按照正常的逻辑，我们可以这么做。**

![20180530172331597](https://s2.loli.net/2024/03/05/5NTGZh1ySmEMLBw.png)

**这有个问题就是，有多少接口，就要多少次代码copy。对于一个“懒人”，这是不可容忍的。好，提出一个公共方法，每个接口都来调用这个接口。这里有点切面的味道了。**

![20180530172427993](https://s2.loli.net/2024/03/05/RSFoXvfkKUNl6x2.png)

**同样有个问题，我虽然不用每次都copy代码了，但是，每个接口总得要调用这个方法吧，这时接口仍不能只需要关心具体的业务，反而还需要关注其他非该接口应该关注的逻辑或处理。于是就有了切面的概念，我将方法注入到接口调用的某个地方（切点）。红框处，就是面向切面编程。**

![20180530172528617](https://s2.loli.net/2024/03/05/sFNtnJH5h4IUm7k.png)

这里还是先给出一个比较专业的概念定义：

Aspect（切面）： Aspect 声明类似于 Java 中的类声明，在 Aspect 中会包含着一些 Pointcut 以及相应的 Advice。
Joint point（连接点）：表示在程序中明确定义的点，典型的包括方法调用，对类成员的访问以及异常处理程序块的执行等等，它自身还可以嵌套其它 joint point。
Pointcut（切点）：表示一组 joint point，这些 joint point 或是通过逻辑关系组合起来，或是通过通配、正则表达式等方式集中起来，它定义了相应的 Advice 将要发生的地方。
Advice（增强）：Advice 定义了在 Pointcut 里面定义的程序点具体要做的操作，它通过 before、after 和 around 来区别是在每个 joint point 之前、之后还是代替执行的代码。
Target（目标对象）：织入 Advice 的目标对象.。
Weaving（织入）：将 Aspect 和其他对象连接起来, 并创建 Adviced object 的过程

![20180530175605692](https://s2.loli.net/2024/03/05/gwYMXzW7ZSKBtQJ.png)

# 3.Spirng项目中的IOC实现（DI依赖注入）

## (i)基于配置文件的DI实现

一个address类

```java
public class Address {

    private String homeAddress;
    private String schoolAddress;

    public void setHomeAddress(String homeAddress) {
        this.homeAddress = homeAddress;
    }

    public void setSchoolAddress(String schoolAddress) {
        this.schoolAddress = schoolAddress;
    }

    @Override
    public String toString() {
        return "Address{" +
                "homeAddress='" + homeAddress + '\'' +
                ", schoolAddress='" + schoolAddress + '\'' +
                '}';
    }
}
```

一个student类

```java

public class Student {
    private String name;
    private int age;
    private School school;
    private Address address;
    public void setAddress(Address address) {
        System.out.println("引用类型address的set方法执行了...");
        this.address = address;
    }
    public void setName(String name) {
        this.name = name;
    }
    public void setAge(int age) {
        this.age = age;
    }
    public void setSchool(School school) {
        this.school = school;
    }
    @Override
    public String toString() {
        return "Student{" +
                "name='" + name + '\'' +
                ", age=" + age +
                ", school=" + school +
                '}';
    }
}
```

一个bean.xml

```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://www.springframework.org/schema/beans
       http://www.springframework.org/schema/beans/spring-beans.xsd">

    <!--
        声明bean（告诉spring要创建某个类的对象）
        1、id：自定义名称，唯一值，spring通过该id的属性值找到对象
        2、class：要创建类的全限定类名
        3、下述的声明语句在spring底层类似与执行了以下代码：
            SomeService service = new SomeServiceImpl();
        4、对象的保存：
            spring将对象保存到内部的map中，map.put(id值，对象)
            map.put("someService",new SomeServiceImpl())
        5、一个bean标签声明一个java对象
        6、spring容器根据bean标签创建对象，尽管存在class属性相同的bean标签，只要是id值不同，
           spring容器就会创建该class的对象
    -->
    <bean id="someService" class="com.mms.service.impl.SomeServiceImpl"/>
    <bean id="someService2" class="com.mms.service.impl.SomeServiceImpl"/>

    <!--
        spring容器也可以创建非自定义类的对象，例如java.lang.String类的对象，只要指定了
        class属性，spring容器就可以创建该类的对象
    -->
    <bean id="myString" class="java.lang.String"/>
</beans>
```

一个测试类

```java
@Test
public void test() {
    //1、指定spring配置文件的名称
    String config = "beans.xml";
    //2、创建表示spring容器的对象 ApplicationContext
    //ClassPathXmlApplicationContext：表示从类路径中加载spring配置文件
    ApplicationContext ac = new ClassPathXmlApplicationContext(config);
    //3、从容器中获取对象
    SomeService service = (SomeService)ac.getBean("someService");
    //4、调用方法
    service.doSome();
}
```

### set注入

在bean.xml中通过setter实现对象赋值

```xml
<!--声明Student对象-->
    <bean id="student" class="com.mms.component.Student">
		<!--
            1、简单类型使用property和value标签给对象属性赋值
            2、简单类型：8个基本类型+String
            3、当spring容器加载到这一行时会在创建完对象的同时使用对象的set方法给属性赋值，底层
               调用的是对象的set方法
            4、spring容器调用的是setXxx()方法，而不管对象是否具有Xxx属性，Xxx不区分大小写
        -->
        <property name="name" value="张三"/>
        <property name="age" value="23"/>
        <!--
            引用类型属性的set注入
            property标签属性
                name：属性名
                ref：引用对象的id值
        -->
        <property name="address" ref="address"/>
    </bean>

    <!--Student对象的引用属性Address-->
    <bean id="address" class="com.mms.component.Address">
        <!--set注入-->
        <property name="homeAddress" value="新疆"/>
        <property name="schoolAddress" value="西安"/>
    </bean>
```

### 构造注入

在bean.xml中通过构造方法直接初始化

```xml
<!--
        构造注入
        1、使用constructor-arg标签完成构造注入
        2、构造注入方式一：根据形参名字
        3、构造注入方式二：根据形参顺序，默认下标从0开始递增
    -->

    <!--根据形参名构造注入，形参的出现顺序不是必须的-->
    <bean id="student" class="com.mms.value.Student">
        <constructor-arg name="name" value="李四"/>
        <constructor-arg name="age" value="24"/>
        <constructor-arg name="address" ref="address"/>
    </bean>
    <bean id="address" class="com.mms.value.Address">
        <constructor-arg name="homeAddress" value="新疆"/>
        <constructor-arg name="schoolAddress" value="西安"/>
    </bean>

    <!--构造注入，使用下标，出现的顺序没要求，因为已经通过下标绑定起来了-->
    <bean id="diByContructor" class="com.mms.value.Student">
        <constructor-arg index="0" value="赵六"/>
        <constructor-arg index="1" value="26"/>
        <constructor-arg index="2" ref="address"/>
    </bean>
```

### 非简单类型的自动注入

对于非简单类型，我们在上面是使用ref属性指向一个非简单类型的对象来完成赋值的，那么当ioc容器每次给一个对象的非简单类型属性赋值时，就要在bean标签内部写一行ref这样的代码，这样会造成重复代码的大量堆积，可以使用引用类型的自动注入。

1. byName形式的引用类型自动注入：通过java对象引用类型的属性名与spring容器中bean标签对象的id值一样且数据类型是一致的，这样能够实现引用类型的自动注入

   ```xml
   <bean id="student" class="com.mms.ba03.Student" autowire="byName">
           <!--简单类型赋值-->
           <property name="name" value="张三"/>
           <property name="age" value="23"/>
       </bean>
       <!--引用类型-->
       <bean id="address" class="com.mms.ba03.Address">
           <property name="homeAddress" value="新疆乌鲁木齐市"/>
           <property name="schoolAddress" value="石河子市"/>
       </bean>
   ```

   java对象引用类型的属性名"address',spring容器中bean标签对象的id值"address",此时可以byName自动注入

2. byType形式的引用类型自动注入：通过java对象引用类型属性的数据类型和spring容器中 bean标签的class属性值是同源关系；
   常见的同源关系：
   1）java引用类型属性数据类型和bean标签的class属性值数据类型一样
   2）java引用类型属性数据类型和bean标签的class属性值数据类型是父子关系
   3）java引用类型属性数据类型和bean标签的class属性值数据类型是接口和实现类关系
   注意：在一个配置文件中，符合条件的同源关系只能有一个

   ```xml
   <!--使用byType实现引用类型自动注入-->
       <bean id="student2" class="com.mms.ba03.Student" autowire="byType">
           <!--简单类型赋值-->
           <property name="name" value="李四"/>
           <property name="age" value="24"/>
       </bean>
       <!--Adress类-->
       <bean id="primarySchool" class="com.mms.ba03.Address">
           <property name="homeAddress" value="新疆乌鲁木齐市"/>
           <property name="schoolAddress" value="石河子市"/>
       </bean>
   ```

   java引用类型属性数据类型"Address",bean标签的class属性值数据类型"Address"，所以符合同源关系，可以byType自动注入

## (ii)基于注解的di实现

下面通过代码来看看@Component注解是怎么实现di的。

```java
@Component(value = "student")
public class Student {
    ...
}
```

该语句就等价为在spring配置文件中进行了以下声明

```xml
<bean id = "student" class = "com.mms.component.Student"/>
```

但是怎么让配置文件知道哪些类是使用注解进行创建对象的呢？需要在配置文件中声明组件扫描器

```xml
<context:component-scan base-package="com.mms.component"/>
```

当spring读取配置文件时，读取到组件扫描器声明语句时，就会去base-package指定的包和其子包下去递归的寻找有注解修饰的类，并根据注解的功能去执行相应的动作

**实现举例**

一个Student类

```java
@Component("student")
public class Student {
    /*引用类型注入（byType方式）
    @Autowired
    private School school;*/

    //引用类型赋值（byName方式）
    @Autowired(required = false)
    @Qualifier(value = "mySchool")
    private School school;
}
```

一个School类

```java
@Component("mySchool")
public class School {

    //注入值
    @Value("西南大学")
    private String schoolAddress;
    @Value("新疆")
    private String homeAddress;

    @Override
    public String toString() {
        return "School{" +
                "schoolAddress='" + schoolAddress + '\'' +
                ", homeAddress='" + homeAddress + '\'' +
                '}';
    }
}
```

@Autowired是spring提供的属性赋值，用于给引用类型赋值，有byName和byType两种方式，默认使用byType方式自动注入
若是要强制至于byName方式，要在@Autowired注解下面加入 @Qualifier(value = “bean的id”)注解，若程序在给引用类型注入时在xml文件中找不到该id的bean标签或者找不到该id的@Component注解，则报错；若不想让程序在赋值失败时报错，可以在@Autowired注解的required属性值置为false



# 4.Spring项目中的AOP实现

## (i)**aspectJ简介**
aspectJ是一个开源的专门做aop的框架。spring框架中集成了aspectj框架，通过spring就能使用aspectj的功能。aspectJ框架实现aop有两种方式：

> 使用xml的配置文件 ： 配置全局事务
> 使用注解，我们在项目中要做aop功能，一般都使用注解， aspectj有5个注解。
> 再使用aspectJ做aop之前要先加入aspectJ依赖。

```xml
<!--aspectJ依赖-->
    <dependency>
      <groupId>org.springframework</groupId>
      <artifactId>spring-aspects</artifactId>
      <version>5.2.5.RELEASE</version>
    </dependency>
```

## (ii)切入点表达式

### 配置方式

1. 内置配置：定义切面通知时，在 @Before 或 @AfterReturning 等通知注解中指定表达式。

```java
@Aspect
@Component
public class DemoAspect {
@Before("execution(* cn.codeartist.spring.aop.advice.*.*(..))")
public void doBefore() {
    // 自定义逻辑
}
```
2. 注解配置
在切面类中，先定义一个方法并使用 @Pointcut 注解来指定表达式。
然后在定义切面通知时，在通知注解中使用 `@Pointcut` 注解来指定表达式。

```java
@Aspect
@Component
public class DemoAspect {
@Pointcut("execution(* cn.codeartist.spring.aop.aspectj.*.*(..))")
private void pointcut() {
    // 切点表达式定义方法，方法修饰符可以是private或public
}

@Before("pointcut()")
public void doBefore(JoinPoint joinPoint) {
    // 自定义逻辑
}
}
```
### 表达式格式

**execution**
匹配方法切入点。根据表达式描述匹配方法，是最通用的表达式类型，可以匹配方法、类、包。

表达式模式：

```
execution(modifier? ret-type declaring-type?name-pattern(param-pattern) throws-pattern?)
```

表达式解释：

**modifier：**匹配修饰符，public, private 等，省略时匹配任意修饰符

**ret-type：**匹配返回类型，使用 * 匹配任意类型

**declaring-type：**匹配目标类，省略时匹配任意类型

.. 匹配包及其子包的所有类
**name-pattern**：匹配方法名称，使用 * 表示通配符

\*匹配任意方法
set* 匹配名称以 set 开头的方法
**param-pattern：**匹配参数类型和数量

() 匹配没有参数的方法
(..) 匹配有任意数量参数的方法
(\*) 匹配有一个任意类型参数的方法
(\*,String) 匹配有两个参数的方法，并且第一个为任意类型，第二个为 String 类型
**throws-pattern：**匹配抛出异常类型，省略时匹配任意类型

使用示例：

```java
// 匹配public方法
execution(public * *(..))
// 匹配名称以set开头的方法
execution(* set*(..))
// 匹配AccountService接口或类的方法
execution(* com.xyz.service.AccountService.*(..))
// 匹配service包及其子包的类或接口
execution(* com.xyz.service..*(..))
```

| 表达式类型  | 描述                               |
| ----------- | :--------------------------------- |
| execution   | 匹配方法切入点                     |
| this        | 匹配代理对象实例的类型             |
| target      | 匹配目标对象实例的类型             |
| args        | 匹配方法参数                       |
| bean        | 匹配 bean 的 id 或名称             |
| within      | 匹配指定类型                       |
| @target     | 匹配目标对象实例的类型是否含有注解 |
| @annotation | 匹配方法是否含有注解               |
| @args       | 匹配方法参数类型是否含有注解       |
| @within     | 匹配类型是否含有注解               |



## (iii)注解方式实现

一个接口IService及其实现类ServiceImpl

```java
public interface IService {
    void doSome(String name, int age);
    Student doStudent(Student student);
}
public class ServiceImpl implements IService {
    @Override
    public void doSome(String name, int age) {
        System.out.println("===doSome()===");
    }
    @Override
    public Student doStudent(Student student) {
        return student;
    }
}
```

一个测试类

```java
public class MyTest {
    //aop前置通知
    @Test
    public void test01() {
        String config = "ba01/applicationContext.xml";
        ApplicationContext ac = new ClassPathXmlApplicationContext(config);
        //获取目标对象，此时的service就是spring生成的代理对象
        //注意返回值类型是接口类型，不能是实现类接口，否则报错
        IService service = (IService) ac.getBean("service");
        //使用代理对象执行方法
        service.doSome("张三",23);
        service.doStudent("张三");
    }
}
```



### @Before前置通知

一个切面声明MyAspectJ 

```java
@Aspect
public class MyAspectJ {

    /**
     * 定义功能增强方法（方法就是切面）
     * 1、方法必须为public
     * 2、方法无返回值
     * 3、方法名称自定义
     * 4、方法可以有参数，也可以没有参数
     * 5、方法的定义上方加入注解，表示切入点的执行时机

    @Before(value = "execution(public void com.mms.ba01.ServiceImpl.doSome(String,int))")
    public void beforeLog() {
        System.out.println("前置通知->系统当前时间：" + new Date());
    }
    */

    /*
        前置通知，带方法参数的切面
        切面方法有参数时要求参数是JoinPoint类型，参数名自定义，该参数就代表了连接点方法，即doSome方法
        使用该参数可以获取切入点表达式、切入点方法签名、目标对象等
     */
    @Before(value = "execution(* *..ServiceImpl.doSome(..))")
    public void beforeLog(JoinPoint jp) {
        System.out.println("连接点方法的方法签名="+jp.getSignature());
        System.out.println("连接点方法的方法名="+jp.getSignature().getName());
        //获取连接点方法参数
        Object[] args = jp.getArgs();
        for (Object arg : args) {
            System.out.println("arg="+arg);
        }
    }
}
```

### @AfterReturning后置通知

一个切面声明MyAspectJ 

```java
@Aspect
public class MyAspectJ {
    /**
     * @AfterReturning: 后置通知，在连接点方法执行之后执行后置通知方法
     * 方法定义格式：
     * 1、公共方法
     * 2、没有返回值
     * 3、方法名称自定义
     * 4、与前置通知一样，可以有JoinPoint类型参数，该参数表示连接点方法对象；还可以有一个
     *    Object类型参数，用于接收连接点方法的执行结果，注意该参数的参数名必须与切入点表达式
     *    的returning属性的属性值一致，表示将returning属性值赋给Object对象
     */
    /*@AfterReturning(value = "execution(* *..ServiceImpl.doOther(..))", returning = "obj")
        public void afterTransaction(JoinPoint jp, Object obj) {
        System.out.println("doOther方法的返回参数="+obj);
        System.out.println("事务已提交...");
        经过验证：在后置通知切面内不能改变连接点方法的返回值
    }*/

    @AfterReturning(value = "execution(* *..ServiceImpl.doStudent(..))", returning = "obj")
    public void afterTransaction(JoinPoint jp, Object obj) {
        System.out.println(obj);
        Student student = new Student();
        student.setName("李四");
        student.setAge(24);
        obj = student;
        System.out.println("===查看是否改变了连接点方法的返回值==="+obj);
        /*
            经过验证：在后置通知切面内不能改变连接点方法的返回值
         */
    }
}
```

### @Around环绕通知

环绕通知是功能最强的通知，它的本质就是jdk动态代理，他可以在连接点方法之前和之后都可以执行，最厉害的是他可以改变连接点方法的执行结果（返回结果）。还是拿上面的doStudent(Student student)方法来说明，经过验证前置通知和后置通知都不能改变doStudent(Student student)方法的返回值。下面看一下环绕通知是如何做的。

一个切面声明MyAspectJ 

```java
@Aspect
public class MyAspectJ {

    /*
        环绕通知：@Around(切入点表达式)
        1、环绕通知是最重要的一个通知，他表示在连接点方法的前或者后都可以执行，它的本质就是jdk动态代理的invoke
           方法的method参数
        2、定义格式
            a、public
            b、必须有返回值，类型为Object

     */
    @Around(value = "pointCut()")

    /*
        再次回忆一下jdk动态代理的invoke方法的定义
        @Override
        public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        是不是感觉与下面的方法定义眼熟啊，没错，环绕通知切面的定义实质上就是jdk动态代理
     */
    public Object around(ProceedingJoinPoint pj) throws Throwable {
        System.out.println("环绕通知在连接点方法之前执行了...");
        Object result = null;
        result = pj.proceed();
        Student student = new Student();
        student.setName("李四");
        student.setAge(24);
        //改变连接点方法返回值
        result = student;
        System.out.println("事务已提交...");
        return result;
    }
    @Pointcut(value = "execution(* *.doStudent(..))")
    public void pointCut() {
        //空方法体
    }
}
```

## (iv)配置文件方法实现

**通过 Spring API 实现**

用MethodBeforeAdvice与AfterReturningAdvice的子类的实例化对象作为通知的方法引用，他们可以通过覆写before或afterReturning来自定义。

```java
public class Log implements MethodBeforeAdvice {
    //method : 要执行的目标对象的方法
    //objects : 被调用的方法的参数
    //Object : 目标对象
    @Override
    public void before(Method method, Object[] objects, Object o) throws Throwable {
        System.out.println( o.getClass().getName() + "的" + method.getName() + "方法被执行了");
    }
}
```

```java
public class AfterLog implements AfterReturningAdvice {
    //returnValue 返回值
    //method被调用的方法
    //args 被调用的方法的对象的参数
    //target 被调用的目标对象
    @Override
    public void afterReturning(Object returnValue, Method method, Object[] args, Object target) throws Throwable {
        System.out.println("执行了" + target.getClass().getName()
        +"的"+method.getName()+"方法,"
        +"返回值："+returnValue);
    }
}
```

```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xmlns:aop="http://www.springframework.org/schema/aop"
       xsi:schemaLocation="http://www.springframework.org/schema/beans
        http://www.springframework.org/schema/beans/spring-beans.xsd
        http://www.springframework.org/schema/aop
        http://www.springframework.org/schema/aop/spring-aop.xsd">
    <!--注册bean-->
    <bean id="userService" class="com.kuang.service.UserServiceImpl"/>
    <bean id="log" class="com.kuang.log.Log"/>
    <bean id="afterLog" class="com.kuang.log.AfterLog"/>
    <!--aop的配置-->
    <aop:config>
        <!--切入点  expression:表达式匹配要执行的方法-->
        <aop:pointcut id="pointcut" expression="execution(* com.kuang.service.UserServiceImpl.*(..))"/>
        <!--执行环绕; advice-ref执行方法 . pointcut-ref切入点-->
        <aop:advisor advice-ref="log" pointcut-ref="pointcut"/>
        <aop:advisor advice-ref="afterLog" pointcut-ref="pointcut"/>
    </aop:config>
</beans>
```

**自定义类来实现Aop**

aop:aspect ref="diy"启用自定义，aop:before与aop:after通过method调用diy这个bean中的方法

```java
public class DiyPointcut {
    public void before(){
        System.out.println("---------方法执行前---------");
    }
    public void after(){
        System.out.println("---------方法执行后---------");
    }
}
```

```xml
<!--第二种方式自定义实现-->
<!--注册bean-->
<bean id="diy" class="com.kuang.config.DiyPointcut"/>
<!--aop的配置-->
<aop:config>
    <!--第二种方式：使用AOP的标签实现-->
    <aop:aspect ref="diy">
        <aop:pointcut id="diyPonitcut" expression="execution(* com.kuang.service.UserServiceImpl.*(..))"/>
        <aop:before pointcut-ref="diyPonitcut" method="before"/>
        <aop:after pointcut-ref="diyPonitcut" method="after"/>
    </aop:aspect>
</aop:config>
```

# 5.项目所需依赖

```xml
<dependency>
    <groupId>org.springframework</groupId>
    <artifactId>spring-webmvc</artifactId>
    <version>5.1.10.RELEASE</version>
</dependency>
```
