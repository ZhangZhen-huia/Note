# 一、Git相关配置

查看配置    **`git config -l`** 

查看系统config    **`git config --system --list`** 

查看当前用户（global）配置 **`git config --global --list`**



## **系统配置文件存放处**

![image-20240812210538722](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122105758.png)

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122105736.png" alt="image-20240812210552688" style="zoom:50%;" />

## **用户配置文件所在地**

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122106381.png" alt="image-20240812210639336" style="zoom:67%;" />

## 用户名称和e-mail地址

当你安装Git后首先要做的事情就是这个。这是非常重要的，因为每次Git提交都 会使用该信息。它被永远的嵌入到了你的提交中：

git config --global user.name "zhang-noob"    				  名称

git config --global user.email 3466493941@qq.com 			邮箱

**<u>而这两个可以更改</u>**

# 二、初始化仓库

## `git init`——初始化仓库

## `git status`——查看仓库的状态

作两次对比，**工作区** vs **暂存区** 以及 **暂存区** vs **仓库**，并将两次对比的结果显示在输出中。哪个对比没差别，就不显示。两个对比都没差别，就显示working tree clean。



## `git add`——向暂存区中添加文件

- git add . 添加文件夹中的所有文件
- git add "要添加的东西.文件格式"

 

## `git commit`——保存仓库的历史记录

- 记述一行提交信息--->git commit -m "message" ,"message" 可以是一些备注信息
- 记述详细提交信息--->直接执行 git  commit命令,输入完成后记得保存一下，不然没用

![image-20240812211115943](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122111974.png)

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122111823.png" alt="image-20240812211125782" style="zoom: 67%;" />

==若是我们修改了东西但是没有add就commit，那么新修改的东西就不会被传到仓库==

如下

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122112691.png" alt="image-20240812211242638" style="zoom: 50%;" />

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122113049.png" alt="image-20240812211305001" style="zoom:50%;" />

然后`git commit -m` "添加新的节点到链表" 

然后`git diff HEAD -- test.c` 查看工作区和暂态区的差别发现第二次的修改未提交上去

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122114943.png" alt="image-20240812211424906" style="zoom:67%;" />

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122114721.png" alt="image-20240812211438684" style="zoom: 67%;" />



# 三、**版本回退与文件修改**

## `git log`——查看提交日志

- 只显示提交信息的第一行--->`git log --pretty=short`

- 只显示指定目录、文件的日志----->在 git log命令后加上目录名 如 `git log README.md`

- 显示文件的改动 `git log -p`       如：git log -p README.md   按q键退出查看

- 如果嫌输出信息太多，看得眼花缭乱的，可以试试 `git log --pretty=oneline`

  

## `git reflog` 查看以往的各个操作的哈希值 



## `git rest --hard`（哈希值）回溯历史版本

- 先`git reflog` 查看以往的各个操作的哈希值 或者直接用`git log`也行

​	这个很好用，比如我们回退了历史，但是我们已经把这个**终端**关了，此时想回到最新版本就可以用这个查找哈希值

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122118336.png" alt="image-20240812211816294" style="zoom:67%;" />

- 然后在`git reset --hard` c9a9ed2 ***回溯历史版本***

  ![image-20240812212233671](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122122703.png)

- 也可以直接 `**git reset --hard HEAD^**`

> [!NOTE]
>
> 在Git中，用HEAD表示当前版本，上一个版本就是HEAD^，上上一个版本就是HEAD^^，当然往上100个版本写100个^比较容易数不过来，所以写成HEAD~100。
>
> --hard参数有啥意义？--hard会回退到上个版本的已提交状态，而--soft会回退到上个版本的未提交状态，--mixed会回退到上个版本已添加但未提交的状态。

**<u></u>**

**<u>*然后用linux操作 `cat` 文件 来查看文件是否回到了之前的版本*</u>**

​		此时若是还想回到最新版本，就 git reset     --hard 哈希值 就行了,版本号没必要写全，前几位就可以了，Git会自动去找



## `git diff`——查看更改前后的差别，查看工作区和暂存区差异

`git diff --cached`					查看暂存区和仓库差异

`git diff HEAD --` (要查看的文件)		查看工作区和仓库的差异

git add的反向命令`git checkout`		  撤销工作区修改，即把暂存区最新版本转移到工作区，

git commit的反向命令`git reset HEAD` 	就是把仓库最新版本转移到暂存区。



> [!IMPORTANT]
>
> <img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122131967.png" alt="image-20240812213112905" style="zoom:67%;" />



## `git checkout -- file`	丢弃工作区的修改

但是本地文件不会被修改

其本质其实是用版本库里的版本替换工作区的版本，无论工作区是修改还是删除，都可以“一键还原”

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122133560.png" alt="image-20240812213318516" style="zoom:67%;" />



## `rm` 文件  删除文件

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122134925.png" alt="image-20240812213433882" style="zoom: 67%;" />

***然后我们确实要删除test1.c 那就再git rm 彻底删除，然后再git commit***

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122135626.png" alt="image-20240812213506583" style="zoom:67%;" />



# 四、远程仓库

==创建仓库之前我们要先添加SSH密钥，先去用户目录下看看有没有.ssh的文件夹==

## 获取密钥

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122136046.png" alt="image-20240812213614003" style="zoom:67%;" />

**若没有，那就需要先获取密钥**

**ssh-keygen -t rsa -C "yourmail@example.com"**

****

**然后会在该目录下生成两个文件**

**id_rsa是私钥，不能泄露出去，id_rsa.pub是公钥，也是我们需要上传到github或者gitee上的密钥可以放心地告诉任何人。**

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122139724.png" alt="image-20240812213933606" style="zoom:67%;" />

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122140785.png" alt="image-20240812214001729" style="zoom: 67%;" />



## 创建仓库

这样密钥就获取成功了，接下来在github上创建一个仓库

然后在终端输入

**`git remote add origin git@github.com:michaelliao/learngit.git`**

michaelliao替换成你自己的GitHub账户名

远程库的名字就是origin，这是Git默认的叫法，也可以改成别的，但是origin这个名字一看就知道是远程

 

------

## 推送远程

下一步，就可以把本地库的所有内容推送到远程库上：

 **`git push -u origin main`**

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122142542.png" alt="image-20240812214200490" style="zoom:67%;" />

然而在提交的时候若是出现如下问题

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122143467.png" alt="image-20240812214300421" style="zoom:67%;" />

***依次输入以下命令就好了，目前不知道为什么***

```c
git fetch origin main

git merge origin FETCH_HEAD

git pull --rebase origin main
```



## 查看远程库信息

**`git remote -v`**



## 删除远程库 

`git remote rm` + 名字 命令

先用**`git remote -v`**查看远程库信息

![image-20240812214444724](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122144759.png)

**`git remote rm origin`**就行了，不过这只是删除了本地和远程的联系，真正删除远程仓库需要去GitHub上



## 推送抓取分支

**`git push origin main`**(这里选择要推送的分支)

**抓取分支就是clone,pull**

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122149548.png" alt="image-20240812214941497" style="zoom:67%;" />

## 小结

![image-20240812215029434](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122150493.png)



# 五、[Rebase](https://liaoxuefeng.com/books/git/branch/rebase/index.html)

- ***rebase 操作可以把本地未push的分叉提交历史整理成直线；***
- ***rebase 的目的是使得我们在查看历史提交的变化时更容易，因为分叉的提交需要三方对比。***



# 六、分支操作



##  `git branch`——显示分支一览表

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122153212.png" alt="image-20240812215357161" style="zoom:50%;" />

**‘*’**表示当前所在分支,结果中没有显 示其他分支名，表示本地仓库中只存在 main一个分支。

## `git checkout -b example`——创建、切换分支（example自起）

切换回上一个分支---->**`git checkout -`**

![image-20240812215636532](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122156602.png)



## **`git branch -d [branch-name]`**  删除分支





## 删除远程分支

**`git push origin --delete [branch-name] `**

**`git branch -dr [remote/branch]`**  



## **`git merge example`**  合并分支 

将指定的分支合并到当前分支上 。即将 example 分支上更改的 内容合并到 main 分支上

### 	**`	git merge --no-ff feature-A`**

​		随后编辑器会启动，用于录入合并提交的信息，默认信息中已经包含了是从 feature-A 分支合并过来的相关内容，所 以可不必做任何更改。将编辑器中显示的内容保存，关闭编辑器

​		<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122159132.png" alt="image-20240812215950075" style="zoom:50%;" />

### 	**`git merge --no-ff -m " "`**

​	git merge --no-ff     -m "merge with no-ff" dev 不使用fast forward合并，因为fast     forward合并，删除分支后，会丢掉分支信息



## **`git log --graph`** 以图表形式查看分支

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122201120.png" alt="image-20240812220117999" style="zoom: 33%;" />

## **解决分支冲突**

![image-20240812220324974](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122203056.png)

**`git log --graph --pretty=oneline --abbrev-commit`**



用**`git log --graph`**命令可以看到分支合并图。

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122203318.png" alt="image-20240812220352271" style="zoom: 67%;" />



## 存储当前分支工作现场

**`git stash`** ，**我当前分支的任务还没有完成，只是add了并不想commit，所以就先保存一下**

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122205224.png" alt="image-20240812220520179" style="zoom: 67%;" />

然后切换到其他分支上去干其他的事情，干完之后再回来，但是发现工作区是干净的，所以需要恢复

先用**`git stash list`**查看

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122205388.png" alt="image-20240812220547340" style="zoom:67%;" />

一是用**`git stash apply`**恢复，***但是恢复后，stash内容并不删除，你需要用`git stash drop`来删除；***



另一种方式是用**`git stash pop`**，恢复的同时把stash内容也删了

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122206480.png" alt="image-20240812220629432" style="zoom:67%;" />

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122206850.png" alt="image-20240812220638801" style="zoom:67%;" />

![image-20240812220652492](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122206545.png)



## 复制

![image-20240812220708018](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122207102.png)



# 七、标签管理

首先，切换到需要打标签的分支上，然后 **`git tag "name"`**就可以打一个标签

 

可以用命令**`git tag`**查看所有标签

 

也可以后加commit id号进行打标签：git tag v0.9 f52c633

 

可以用**`git show  "tagname`**"来看标签的信息

 

还可以创建带有说明的标签，用-a指定标签名，-m指定说明文字：

**`git tag -a v0.1 -m "version 0.1 released" 1094adb`**

 ![image-20240812220828090](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408122208140.png)

 

 

远程

- 命令**`git push origin "tagname"`**可以推送一个本地标签；

- 命令**`git push origin --tags`** 可以推送全部未推送过的本地标签；

- 命令**`git tag -d "tagname"`**可以删除一个本地标签；

- 命令**`git push origin :refs/tags/"tagname"`**可以删除一个远程标签。

  ------

  

***若是要删除远程的就需要先删除本地的，然后再删除远程的即***

先**git tag -d "tagname"**

再**git push origin :refs/tags/"tagname"**



# 八、Git也支持Linux操作

```c
1）cd : 改变目录。

2）cd . . 回退到上一个目录，直接cd进入默认目录

3）pwd : 显示当前所在的目录路径。 

4）ls(ll): 都是列出当前目录中的所有文件，只不过ll(两个ll)列出的内容更为详细。 

5）touch : 新建一个文件 如 touch index.js 就会在当前目录下新建一个index.js文件。 

6）rm: 删除一个文件, rm index.js 就会把index.js文件删除。 

7）mkdir: 新建一个目录,就是新建一个文件夹。 

8）rm -r : 删除一个文件夹, rm -r src 删除src目录 

9）mv 移动文件, mv index.html src index.html 是我们要移动的文件, src 是目标文件夹,

当然, 这样写, 必须保证文件和目标文件夹在同一目录下。 

10）reset 重新初始化终端/清屏。 

11）clear 清屏。 

12）history 查看命令历史。

13）help 帮助。 

14）exit 退出。 

15）#表示注释
```

