" ��һ������tab��Ϊ4���ո񣬵ڶ������õ���֮�佻��ʱʹ��4���ո� 
set tabstop=4
set shiftwidth=4

" �ÿո���滻�Ʊ�� 
set expandtab

" ����ƥ��ģʽ�����Ƶ�����һ��������ʱ��ƥ����Ӧ���Ǹ������� 
set showmatch

" ȥ��vim��GUI�汾�е�toolbar 
" set guioptions+=T
" set guioptions+=m

" ��������ɫ���� 
if has('gui_running')
    set background=light
else
    set background=dark
endif

" ������ɫ������ѡ�����desert��Ҳ��������������vim������:color ����tab�����Բ鿴 
" color desert
" colorscheme solarized

"��˵�е�ȥ���߿����±���һ��
set go=

" ��ʱ���Ļ���ʾ���룬��һ�¼��������� 
let &termencoding=&encoding
set fileencodings=utf-8,gb18030,gbk

" ��������
set hlsearch

" GUI����ʹ���Զ�������
if has('gui_running')
    set guifont=DejaVu_Sans_Mono:h12:cDEFAULT,WenQuanYi_Micro_Hei:h12:cDEFAULT,Microsoft_Yahei:h12:cDEFAULT,Yahei_Mono:h12:cDEFAULT
     set guifontwide=DejaVu_Sans_Mono:h12:cDEFAULT,WenQuanYi_Micro_Hei_Mono:h12:cDEFAULT,Yahei_Mono:h12:cDEFAULT,Microsoft_Yahei_UI:h12:cDEFAULT
endif

