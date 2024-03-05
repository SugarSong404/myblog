title: vue学习02 - vue的各类常用模块

tags:    

- "vue"
- "异步请求"   

categories:

- "web"
- "vue框架"

thumbnail: https://s2.loli.net/2024/03/05/fwhXHGV1Tex4vRU.png

---

# 

# 1.vue路由模块

`npm install vue-router -g`(vue-cli可以自动安装)

## 路由组件

 `router-link` ：请注意，我们没有使用常规的 `a` 标签，而是使用这个自定义组件来创建链接。这使得 Vue Router 可以在不重新加载页面的情况下更改 URL，处理 URL 的生成以及编码。我们将在后面看到如何从这些功能中获益。

`router-view` 将显示与 URL 对应的组件。你可以把它放在任何地方，以适应你的布局。

**一个示例**

```vue
<div id="app">
  <h1>Hello App!</h1>
  <p>
    <!--使用 router-link 组件进行导航 -->
    <!--通过传递 `to` 来指定链接 -->
    <!--`<router-link>` 将呈现一个带有正确 `href` 属性的 `<a>` 标签-->
    <router-link to="/">Go to Home</router-link>
    <router-link to="/about">Go to About</router-link>
  </p>
  <!-- 路由出口 -->
  <!-- 路由匹配到的组件将渲染在这里 -->
  <router-view></router-view>
</div>
<script>
// 1. 定义路由组件.
// 也可以从其他文件导入
const Home = { template: '<div>Home</div>' }
const About = { template: '<div>About</div>' }

// 2. 定义一些路由
// 每个路由都需要映射到一个组件。
// 我们后面再讨论嵌套路由。
const routes = [
  { path: '/', component: Home },
  { path: '/about', component: About },
]
// 3. 创建路由实例并传递 `routes` 配置
// 你可以在这里输入更多的配置，但我们在这里
// 暂时保持简单
const router = VueRouter.createRouter({
  routes, // `routes: routes` 的缩写
})
// 5. 创建并挂载根实例
const app = Vue.createApp({})
//确保 _use_ 路由实例使
//整个应用支持路由。
app.use(router)
app.mount('#app')
</script>
```

可以配置router的history属性来选择不同的历史记录模式，如hash模式,memory模式，html5模式，默认为hash模式

## 带参数的动态路由匹配

**应用场景**：

现有很多活动，活动显示的页面`/activity`都是同一个，只是根据id不同显示不同的活动数据如`/activity/1`，`/activity/2`。我们不需要写很多个不同的显示组件与路由，只需要在`/activity`路由下传参即可

**实现方式**：

```js
const Activity = {
  template: '<div>Activity {{ $route.params.id }}</div>',
}
// 这些都会传递给 `createRouter`
const routes = [
  // 动态字段以冒号开始
  { path: '/activity/:id', component: Activity },
]
```

此时URL输入`/activity/123`页面就会显示`Activity 123`

**注意事项**：

使用带有参数的路由时需要注意的是，当用户从 `/activity/1` 导航到 `/activity/2` 时，**相同的组件实例将被重复使用**。因为两个路由都渲染同个组件，比起销毁再创建，复用则显得更加高效。**不过，这也意味着组件的生命周期钩子不会被调用**。

要对同一个组件中参数的变化做出响应的话，你可以简单地**监听**`$route` 对象上的任意属性，在这个场景中，就是 `$route.params` ：

```js
const Activity = {
  template: '...',
  created() {
    this.$watch(
      () => this.$route.params,
      (toParams, previousParams) => {
        // 对路由变化做出响应...
      }
    )
  },
}
```

## 嵌套路由

**应用场景：**

在活动页面下有活动内容页面与活动报名页面，及`/activity/:id/content`与`/activity/:id/register`。两个页面的差异对应着`Content`与`Register`两个不同组件，但在差异的同时具有共性(活动的标题、活动的页首页尾等等)。如果我们在`/activity/:id`对应的`Activity`内部模板上写一个`<router-view/>`，那么就可以在保持共性的情况下利用`<router-view/>`映射不同组件了，而不是整体的重写两个页面。

**实现方式：**

```js
const Activity = {
  template: 
    '<div>\n'+
    	'<h1>Activity {{ $route.params.id }}</h1>\n'+ //共性部分，标题
    	'<router-view></router-view>\n'	//关键部分在此,差异只会在这里呈现
    '</div>'+
  
}
// 这些都会传递给 `createRouter`
const routes = [
  // 动态字段以冒号开始
  { path: '/activity/:id',
  	component: Activity,
  	children: [
      {
        path: 'content',
        component: Content,
      },
      {
        path: 'register',
        component: Register,
      },
    ],
  },
]
```

**注意事项：**

此时，按照上面的配置，当你访问 `/activity/1/` 时，在 `Activity` 的 `router-view` 里面什么都不会呈现，因为没有匹配到嵌套路由。也许你确实想在那里渲染一些东西(比如一个404组件)。在这种情况下，你可以提供一个空的嵌套路径：

```js
const routes = [
  // 动态字段以冒号开始
  { path: '/activity/:id',
  	component: Activity,
  	children: [
      {
        path: 'content',
        component: Content,
      },
      {
        path: '',
        component: NotFound,
      },
    ],
  },
]
```

## 命名视图

**应用场景：**

在活动页面下会有导航栏部分，侧索引栏，主要内容板块等一系列同级视图。这些视图与`/activity/:id`的默认视图并不是嵌套关系，并且应该同时显示。

**实现方式：**

```vue
<router-view name="nav"></router-view>
<router-view></router-view><!--这个没有name属性的是默认视图-->
<router-view name="LeftSidebar"></router-view>
```

```js
const router = createRouter({
  history: createWebHashHistory(),
  routes: [
    {
      path: '/',
      components: {
        default: Home,
        nav:Navigationbar,
        LeftSidebar,
      },
    },
  ],
})
```

**注意事项：**

我们也有可能使用命名视图创建嵌套视图的复杂布局。这时你也需要命名用到的嵌套 `router-view` 组件。

## 编程式导航与命名路由

**编程式导航**

| 声明式                     | 编程式             |
| :------------------------- | ------------------ |
| `<router-link :to="..." >` | `router.push(...)` |

| 声明式                            | 编程式                |
| :-------------------------------- | :-------------------- |
| `<router-link :to="..." replace>` | `router.replace(...)` |

| 编程式                                        | 编程式                |
| :-------------------------------------------- | :-------------------- |
| `router.push({ path: '...', replace: true })` | `router.replace(...)` |

使用push在history中添加了新条目，可以用router.go(-1)返回；而replace则代替了当前条目

**命名路由**

如果有

```js
const Activity = {
  template: '<div>Activity {{ $route.params.id }}</div>',
}
// 这些都会传递给 `createRouter`
const routes = [
  // 动态字段以冒号开始
  { 
  name:'act',
  path: '/activity/:id',
  component: Activity 
  },
]
```

那么router.push({ name: 'act', params: { id: '1' } })等价于访问`/activity/1`

**重定向**

```js
const routes = [{ path: '/activity/:id/content', redirect: '/activity/:id/register' }]
```

上面的例子中访问内容页面会直接重定向到访问报名活动上

也可以如下实现

```js
const routes = [
  {
    path: '/activity/:id/content',
    redirect: to => {
      // 该函数接收path作为参数,to.path='/activity/:id/content'
      return 'register'
    },
  },
]
```

**别名**

重定向是切换URL，显示重定向的页面；而别名是不切换URL，显示别名对应的真实路径的页面。

如下代码，在`/people`与`/users/list`下都会显示和`/users/ul`下一样的内容

```
const routes = [
  {
    path: '/users',
    component: UsersLayout,
    children: [
      { path: 'ul', component: UserList, alias: ['/people', 'list'] },
    ],
  },
]
```

## 路由组件传参

总是通过`$route.params`传参，与路由耦合性太高

**布尔模式：**于是当router的`props` 属性设置为 `true` 时，`route.params` 将被设置为组件的 props。即：

```js
const User = {
  template: '<div>User {{ $route.params.id }}</div>'
}
const routes = [{ path: '/user/:id', component: User }]
```

可以替换为

```js
const User = {
  // 请确保添加一个与路由参数完全相同的 prop 名
  props: ['id'],
  template: '<div>User {{ id }}</div>'
}
const routes = [{ path: '/user/:id', component: User, props: true }]
```

**注意事项：**

对于有命名视图的路由，你必须为每个命名视图定义 `props` 配置：

```js
const routes = [
  {
    path: '/user/:id',
    components: { default: User, sidebar: Sidebar },
    props: { default: true, sidebar: false }
  }
]
```

另外还有对象模式与函数模式两种模式可以了解

## 路由钩子(守卫)

参数：

`to`：路由将要跳转的路径信息
`from`：路径跳转前的路径信息
`next`：路由的控制参数
`next()` 跳入下一个页面
`next(’/path’)` 改变路由的跳转方向，使其跳到另一个路由
`next(false)` 返回原来的页面
`next((vm)=>{})` 仅在 beforeRouteEnter 中可用，vm 是组件实例，因为beforeRouteEnter 拿不到this

**全局钩子**

- 你可以使用 `router.beforeEach` 注册一个全局前置守卫：

```js
const router = createRouter({ ... })
router.beforeEach((to, from) => {
  // 返回 false 以取消导航
  return false
})
```

当一个导航触发时，全局前置守卫按照创建顺序调用。守卫是异步解析执行，此时导航在所有守卫 resolve 完之前一直处于**等待中**。

- 你可以用 `router.beforeResolve` 注册一个全局守卫。这和 `router.beforeEach` 类似，因为它在**每次导航**时都会触发，不同的是，解析守卫刚好会在导航被确认之前、**所有组件内守卫和异步路由组件被解析之后**调用。

- 你也可以注册全局后置钩子,它们对于分析、更改页面标题、声明页面等辅助功能以及许多其他事情都很有用。

```js
router.afterEach((to, from) => {
  sendToAnalytics(to.fullPath)
})
```



**组件注册钩子传递给路由**

路由跳转到该组件时会触发

```js
const UserDetails = {
  template: `...`,
  beforeRouteEnter(to, from) {
    // 在渲染该组件的对应路由被验证前调用
    // 不能获取组件实例 `this` ！
    // 因为当守卫执行时，组件实例还没被创建！
  },
  beforeRouteUpdate(to, from) {
    // 在当前路由改变，但是该组件被复用时调用
    // 举例来说，对于一个带有动态参数的路径 `/users/:id`，在 `/users/1` 和 `/users/2` 之间跳转的时候，
    // 由于会渲染同样的 `UserDetails` 组件，因此组件实例会被复用。而这个钩子就会在这个情况下被调用。
    // 因为在这种情况发生的时候，组件已经挂载好了，导航守卫可以访问组件实例 `this`
  },
  beforeRouteLeave(to, from) {
    // 在导航离开渲染该组件的对应路由时调用
    // 与 `beforeRouteUpdate` 一样，它可以访问组件实例 `this`
  },
}
```

**路由独享的守卫**

除了全局与组件传入的方式，路由本身就可以配置钩子

`beforeEnter` 守卫 **只在进入路由时触发**，不会在 `params`、`query` 或 `hash` 改变时触发。例如，从 `/users/2` 进入到 `/users/3` 或者从 `/users/2#info` 进入到 `/users/2#projects`。它们只有在 **从一个不同的** 路由导航时，才会被触发。

你也可以将一个函数数组传递给 `beforeEnter`，这在为不同的路由重用守卫时很有用：

```js
function removeQueryParams(to) {
  if (Object.keys(to.query).length)
    return { path: to.path, query: {}, hash: to.hash }
}

function removeHash(to) {
  if (to.hash) return { path: to.path, query: to.query, hash: '' }
}
const routes = [
  {
    path: '/users/:id',
    component: UserDetails,
    beforeEnter: [removeQueryParams, removeHash],
  },
  {
    path: '/about',
    component: UserDetails,
    beforeEnter: [removeQueryParams],
  },
]
```

**☆完整的导航解析流程**

1. 导航被触发。
2. 在失活的组件里调用 `beforeRouteLeave` 守卫。
3. 调用全局的 `beforeEach` 守卫。
4. 在**重用**的组件里调用 `beforeRouteUpdate` 守卫。
5. 在路由配置里调用 `beforeEnter`。
6. 解析异步路由组件。
7. 在被激活的组件里调用 `beforeRouteEnter`。
8. 调用全局的 `beforeResolve` 守卫。
9. 导航被确认。
10. 调用全局的 `afterEach` 钩子。
11. 触发 DOM 更新。
12. 调用 `beforeRouteEnter` 守卫中传给 `next` 的回调函数，创建好的组件实例会作为回调函数的参数传入。

# 2.vue异步通信

`cnpm install --save vue-axios`

## 发送请求的方式

**用方法别名发POST请求**

```js
const apiUrl = '/user';
const postData = {
  key1: 'value1',
  key2: 'value2'
};
axios.post(apiUrl, postData)
  .then(response => {
    // Handle successful response
    console.log('Response:', response.data);
  })
  .catch(error => {
    // Handle error
    console.error('Error:', error);
  });

```

**用方法别名发起GET请求**

```js
const apiUrl = '/user';
const params = {
  ID: 12345
};
axios.get(apiUrl, {
  params: params
})
  .then(response => {
    console.log('Response:', response.data);
  })
  .catch(error => {
    console.error('Error:', error);
  });

```

**执行多个并发请求**

```js
function getUserAccount() {
  return axios.get('/user/12345');
}
 
function getUserPermissions() {
  return axios.get('/user/12345/permissions');
}
// 发送多个并发请求
axios.all([getUserAccount(), getUserPermissions()])
  .then(axios.spread(function (acct, perms) {// 处理多个请求的响应数据
    // 两个请求现在都执行完成
  }));
```

**用axios相关配置发送请求**

axios(config)

```js
// 发送 POST 请求
axios({
  method: 'post',
  url: '/user/12345',
  data: {
    firstName: 'Fred',
    lastName: 'Flintstone'
  }
});
//  GET 请求远程图片
axios({
  method:'get',
  url:'http://bit.ly/2mTM3nY',
  responseType:'stream'
  params: {
    // 添加参数到请求URL中
    key1: 'value1',
    key2: 'value2'
  }
})
  .then(function(response) {
  response.data.pipe(fs.createWriteStream('ada_lovelace.jpg'))
});
```

axios(url[, config])

```js
// 发送 GET 请求（默认的方法）
axios('/user/12345');
```

所有请求方法的别名

为方便使用，官方为所有支持的请求方法提供了别名，可以直接使用别名来发起请求：

```js
axios.request(config)
axios.get(url[, config])
axios.delete(url[, config])
axios.head(url[, config])
axios.post(url[, data[, config]])
axios.put(url[, data[, config]])
axios.patch(url[, data[, config]])
```

## 请求配置项

可以使用自定义配置新建一个 axios 实例：axios.create([config])

```js
const instance = axios.create({
  baseURL: 'https://some-domain.com/api/',
  timeout: 1000,
  headers: {'X-Custom-Header': 'foobar'}
});
```

请求配置项

只有 url 是必需的。如果没有指定 method，请求将默认使用 get 方法

```js
{
  // `url` 是用于请求的服务器 URL
  url: "/user",
  // `method` 是创建请求时使用的方法
  method: "get", // 默认是 get
  // `baseURL` 将自动加在 `url` 前面，除非 `url` 是一个绝对 URL。
  // 它可以通过设置一个 `baseURL` 便于为 axios 实例的方法传递相对 URL
  baseURL: "https://some-domain.com/api/",
  // `transformRequest` 允许在向服务器发送前，修改请求数据
  // 只能用在 "PUT", "POST" 和 "PATCH" 这几个请求方法
  // 后面数组中的函数必须返回一个字符串，或 ArrayBuffer，或 Stream
  transformRequest: [function (data) {
    // 对 data 进行任意转换处理
    return data;
  }],
  // `transformResponse` 在传递给 then/catch 前，允许修改响应数据
  transformResponse: [function (data) {
    // 对 data 进行任意转换处理

    return data;
  }],
  // `headers` 是即将被发送的自定义请求头
  headers: {"X-Requested-With": "XMLHttpRequest"},
  // `params` 是即将与请求一起发送的 URL 参数
  // 必须是一个无格式对象(plain object)或 URLSearchParams 对象
  params: {
    ID: 12345
  },
  // `paramsSerializer` 是一个负责 `params` 序列化的函数
  // (e.g. https://www.npmjs.com/package/qs, http://api.jquery.com/jquery.param/)
  paramsSerializer: function(params) {
    return Qs.stringify(params, {arrayFormat: "brackets"})
  },
  // `data` 是作为请求主体被发送的数据
  // 只适用于这些请求方法 "PUT", "POST", 和 "PATCH"
  // 在没有设置 `transformRequest` 时，必须是以下类型之一：
  // - string, plain object, ArrayBuffer, ArrayBufferView, URLSearchParams
  // - 浏览器专属：FormData, File, Blob
  // - Node 专属： Stream
  data: {
    firstName: "Fred"
  },
  // `timeout` 指定请求超时的毫秒数(0 表示无超时时间)
  // 如果请求花费了超过 `timeout` 的时间，请求将被中断
  timeout: 1000,
  // `withCredentials` 表示跨域请求时是否需要使用凭证
  withCredentials: false, // 默认的
  // `adapter` 允许自定义处理请求，以使测试更轻松
  // 返回一个 promise 并应用一个有效的响应 (查阅 [response docs](#response-api)).
  adapter: function (config) {
    /* ... */
  },
  // `auth` 表示应该使用 HTTP 基础验证，并提供凭据
  // 这将设置一个 `Authorization` 头，覆写掉现有的任意使用 `headers` 设置的自定义 `Authorization`头
  auth: {
    username: "janedoe",
    password: "s00pers3cret"
  },
  // `responseType` 表示服务器响应的数据类型，可以是 "arraybuffer", "blob", "document", "json", "text", "stream"
  responseType: "json", // 默认的
  // `xsrfCookieName` 是用作 xsrf token 的值的cookie的名称
  xsrfCookieName: "XSRF-TOKEN", // default
  // `xsrfHeaderName` 是承载 xsrf token 的值的 HTTP 头的名称
  xsrfHeaderName: "X-XSRF-TOKEN", // 默认的
  // `onUploadProgress` 允许为上传处理进度事件
  onUploadProgress: function (progressEvent) {
    // 对原生进度事件的处理
  },
  // `onDownloadProgress` 允许为下载处理进度事件
  onDownloadProgress: function (progressEvent) {
    // 对原生进度事件的处理
  },
  // `maxContentLength` 定义允许的响应内容的最大尺寸
  maxContentLength: 2000,
  // `validateStatus` 定义对于给定的HTTP 响应状态码是 resolve 或 reject  promise 。如果 `validateStatus` 返回 `true` (或者设置为 `null` 或 `undefined`)，promise 将被 resolve; 否则，promise 将被 rejecte
  validateStatus: function (status) {
    return status &gt;= 200 &amp;&amp; status &lt; 300; // 默认的
  },
  // `maxRedirects` 定义在 node.js 中 follow 的最大重定向数目
  // 如果设置为0，将不会 follow 任何重定向
  maxRedirects: 5, // 默认的
  // `httpAgent` 和 `httpsAgent` 分别在 node.js 中用于定义在执行 http 和 https 时使用的自定义代理。允许像这样配置选项：
  // `keepAlive` 默认没有启用
  httpAgent: new http.Agent({ keepAlive: true }),
  httpsAgent: new https.Agent({ keepAlive: true }),
  // "proxy" 定义代理服务器的主机名称和端口
  // `auth` 表示 HTTP 基础验证应当用于连接代理，并提供凭据
  // 这将会设置一个 `Proxy-Authorization` 头，覆写掉已有的通过使用 `header` 设置的自定义 `Proxy-Authorization` 头。
  proxy: {
    host: "127.0.0.1",
    port: 9000,
    auth: : {
      username: "mikeymike",
      password: "rapunz3l"
    }
  },
  // `cancelToken` 指定用于取消请求的 cancel token
  // （查看后面的 Cancellation 这节了解更多）
  cancelToken: new CancelToken(function (cancel) {
  })
}
```

##  拦截器与请求取消

**添加拦截器**

```js
// 添加请求拦截器
const test =  axios.interceptors.request.use(function (config) {
    // 在发送请求之前做些什么
    return config;
  }, function (error) {
    // 对请求错误做些什么
    return Promise.reject(error);
  });

// 添加响应拦截器
axios.interceptors.response.use(function (response) {
    // 对响应数据做点什么
    return response;
  }, function (error) {
    // 对响应错误做点什么
    return Promise.reject(error);
  });
```

**取消拦截器**

```js
axios.interceptors.request.eject(myInterceptor);
```

**使用 cancel token 取消请求**

在 Axios 中，`cancelToken` 是一个配置选项，用于提供请求的取消令牌（CancelToken）。取消令牌是一个包含 `token` 属性的对象，该属性是一个用于取消请求的函数。这允许你在需要时取消一个正在进行的请求。

`axios.isCancel(thrown)` 是 Axios 提供的一个方法，用于检查一个异常对象是否是由取消请求引起的

```js
//可以使用 CancelToken.source 工厂方法创建 cancel token
var source = axios.CancelToken.source();
axios.get('/user/12345', {
  cancelToken: source.token
}).catch(function(thrown) {
  if (axios.isCancel(thrown)) {
    console.log('Request canceled', thrown.message);
  } else {
    // 处理错误
  }
});
// 取消请求（message 参数是可选的）
source.cancel('Operation canceled by the user.');
```

## JavaScript 对象与 JSON

请求时使用 `application/x-www-form-urlencoded`，axios 会默认序列化 JavaScript 对象为 JSON。



# 3.vue项目练习

若非特殊声明，项目都以vue2为例

## 模块导入

需要npm项

`npm install webpack -g`

`npm install webpack-cli -g`

`npm install vue -g`

`npm install vue-cli -g`

`npm install element-ui -g`

`cnpm install sass-loader node-sass`

使用`vue-cli`在目标目录

```bash
vue init webpack 项目名
```

即可在目标目录生成webpack的vue项目

## 基础结构

![image-20240204154223626](https://s2.loli.net/2024/03/05/zu7MrvHihFK5ZO3.png)

1.`index.html`最终显示的是这个web的起始位置(默认文档位置),也就是`localhost:8080/`下的内容,有一个el为**#app**的vue实例挂载到div上

```vue
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1.0">
    <title>vue-test</title>
  </head>
  <body>
    <div id="app"></div>
    <!-- built files will be auto injected -->
  </body>
</html>
```

2.由**#app**向上追溯到`main.js`,此处导入了各类所需要的模块或组件并创建了vue实例,这个vue实例的根组件是**APP**

- vue2

```js
import Vue from 'vue'
import App from './App'
//扫描路由配置
import router from './router'
Vue.use(router);
new Vue({
  el: '#app',
  router,
  render: h => h(App)
})

```

- vue3（取出router部分）

```js
import { createApp } from 'vue';
import App from './App.vue';
createApp(App).mount('#app');
```

3.于是看到`APP.vue`,至此最基本的vue项目实现

```vue
<template>
</template>
<script>
export default {
  name: 'App',
}
</script>
<style>
</style>
```

4.基础的项目实现了之后可以通过路由配置跳转更多页面，所以需要配置`index.js`界面

```js
import Vue from "vue";
import Main from "../views/Main";
import Login from "../views/Login";
import Router from "vue-router";
Vue.use(Router);

export default new Router({
  mode: 'history',
  routes: [
    {
      path: '/main',
      component: Main,
    },
    {
      path: '/login',
      component: Login
    },
  ]
});
```

5.两个例子组件也一并给出

`Main.vue`

```vue
<template>
  <div>
    <h1>首页</h1>
    <router-link to="/login">login</router-link>
    <router-view></router-view>
  </div>
</template>
<script>
export default {
  name: "Main"
}
</script>
<style scoped>
</style>

```

Login.vue

```vue
<template>
  <div>
    <h1>登录页</h1>
    <router-link to="/main">main</router-link>
    <router-view></router-view>
  </div>
</template>
<script>
export default {
  name: "Login",
}
</script>
<style scoped>
</style>
```

6.`assets`存放静态资源，`components`存放组件，`views`存放页面，`router`存放路由,这是一个最一般的vue项目结构