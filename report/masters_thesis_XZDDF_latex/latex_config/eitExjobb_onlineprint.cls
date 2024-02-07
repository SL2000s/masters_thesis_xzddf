%% 
%% eitExjobb.cls
%% By: Stefan Höst, stefan.host@eit.lth.se
%%
%%%%%%%%%%%%%%%%
%% 
%% Version 2016-09
%%
%%%%%%%%%%%%%%%% 
%% This is a class to be used for Master's thesises at the department of
%% Electrical and Information Technology at Lund University,
%% Sweden. It is based on the standard class report.cls. I have tried
%% to make comments so it is easy to find where to make desired
%% changes. 
%% 
%% IMPORTANT!!
%% To avoid confusion: 
%% If you do make changes then also change the name of the file!!! 
%% If I would make changes in the file I would call it e.g. 
%%   SHeitExjobb.cls 
%% Then I must also change the second line in the code to 
%%   \ProvidesClass{eitExjobb}
%% Otherwise LaTeX will give a warning. Put the new file in the same
%% directory as the report and call the new file with 
%%   \documentclass[...]{eitExjobb}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{eitExjobb}
%% Boolean variables used by the class. The standard way I use is to
%% specify a Boolean variable for each option. Then I can use if-else
%% statements in the definitions later.
\newif\ifshfinal\shfinalfalse
%% Options. If final is chosen it rearanges the layout so it can be
%% printed by PH and Johan. Default is to print the text in the middle
%% of the page and then put a frame round it.
\DeclareOption{final}{\shfinaltrue}
%% Process options
\ProcessOptions\relax
%% Load everything in report.cls
\LoadClassWithOptions{report}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% geometry: A package for page layout.
\RequirePackage{geometry}
%%%%%%%%%%%%%%%%%%%%%
%% Page layout depending on option final
\ifshfinal %% Final. Print as book with left side 1in.
  \geometry{paper=a4paper,
    body={125mm,200mm},
    left=29.5mm,
    vmarginratio=1:1}
\else %% Not final. Center the page on the paper and put a frame around it.
  \geometry{papersize={169mm,239mm},%% G5
    body={125mm,200mm},
    left=22.2mm,
    vmarginratio=1:1}
  %% Crop marks (the frame; can also be set to cam and some others)
  % \RequirePackage[frame,a4,dvips,center]{crop}
\fi
%%%%%%%%%%%%%%%%%%%%%
%% How to write standard counters.
\renewcommand\figurename{Figure}
\renewcommand\thefigure{\thechapter.\arabic{figure}}
\renewcommand\tablename{Table}
\renewcommand\thetable{\thechapter.\arabic{table}}
%%%%%%%%%%%%%%%%%%%%%
%% Captions. A package that controlls how the captions (figure and
%% table) are printed.
\RequirePackage{caption}
\captionsetup{margin=10pt,%
  indention=8mm,%
  font={sf,small},%
  labelfont=bf,%
  calcwidth=.8\textwidth}
%%%%%%%%%%%%%%%%%%%%% 
%% Headers and footer. 
\RequirePackage{fancyhdr}
\fancyhead{}  % Empty header
\fancyfoot{}  % Empty footer
\pagestyle{fancy}
%% See to that \rightmark is always the chapter name.
\renewcommand\chaptermark[1]{\markright{#1}} 
\renewcommand\sectionmark[1]{}
%% Page numbering before 'Introduction'. Only small roman numbers at
%% the end of the page.
\newcommand\frontmatter{%
  \renewcommand\headrulewidth{0pt}      % No header line
  \renewcommand\thepage{\roman{page}}   % roman page numbering
  \fancyfoot[CO,CE]{\thepage}  % page number centered in the footer
}
%% Page numbering in the main document. The footer is empty and the
%% header has the page number and the chapter name. 
\newcommand\mainmatter{%
  \setcounter{page}{1}                             % Restart page numbering
  \renewcommand\thepage{\arabic{page}}             % Arabic numbering
  \fancyfoot{}                                     % Empty footer
  \fancyhead[LE,RO]{\small\textsf{\thepage}}       % Page number on outer side of header
  \fancyhead[LO,RE]{\small\textsf{\rightmark}}     % Chapter on inner side of header
  \renewcommand\headrulewidth{0.4pt}               % Line under header
}
%%%%%%%%%%%%%%%%%%%%%
%% Font size changing package
\RequirePackage{relsize}
%%%%%%%%%%%%%%%%%%%%%
%% Section style
\RequirePackage{sectsty}
%% Make all sections sans serif and different sizes.
\sectionfont{\normalsize\relsize{2}\sffamily\mdseries}
\subsectionfont{\normalsize\relsize{1}\sffamily\mdseries}
\subsubsectionfont{\normalsize\relsize{0}\sffamily\mdseries}
%%%%%
%% Chapters style. This was a bit worse. I have redifined the commands
%% \@makechapterhead and \@makeschapterhead from report.cls. The first
%% one controlls the printing of a chapter with numbers and the second
%% without numbers. Before the actual definitions I have some small
%% fixes. First I put a \cleardoublepage in front of chapter to always
%% get a new chapter on an odd (right) side. Then I need a little bit
%% thicker \hrulefill (just redefine  \hrulefill), and then sets
%% \chaptername and \chapterfont. The latter is the font for the
%% chapter name. There should also be font commands for \chaptername
%% and chapter numbering. But tis is for future work
\let\sh@chapter\chapter
\renewcommand\chapter{\cleardoublepage\sh@chapter}   % New chapter on left side
\newcommand\hrulefillchap{\leavevmode\leaders\hrule height 1.0pt \hfill\kern\z@}
\renewcommand\chaptername{Chapter}
\renewcommand\chapterfont{\fontsize{40}{45}\selectfont}
%% chapter
\renewcommand\@makechapterhead[1]{%
  \vspace*{4\p@}%
  {\parindent \z@ \raggedright \normalfont
    \ifnum \c@secnumdepth >\m@ne
      \normalsize\sffamily\mdseries
      \hrulefillchap\space\@chapapp\space\chapterfont\thechapter%
      \par\nobreak
      \vskip 5\p@
    \fi
    \interlinepenalty\@M
    \normalsize\sffamily\mdseries
    \raggedleft\relsize{4}
    #1
    \par\nobreak
    \vskip -7\p@
    \hrulefillchap\par\nobreak
    \vskip 40\p@
  }}
%% chapter*
\renewcommand\@makeschapterhead[1]{%
  \vspace*{40\p@}%
  {\parindent \z@ \raggedright
    \normalfont
    \hrulefillchap
    \par\nobreak
    \vskip 5\p@
    %%
    \interlinepenalty\@M
    \normalsize\sffamily\mdseries
    \raggedleft\relsize{4}
    #1
    \par\nobreak
    \vskip -7\p@
    \hrulefillchap\par\nobreak
    \vskip 40\p@
  }}
%%%%%%%%%%%%%%%%%%%%%
%% Toc style. 
%% Set by the tocloft package and set the title of the ToC, LoF and LoT. 
%% Here only chapters and sections in the ToC, from tocdepth=1
%% (0=chapter, 1=sections, 2=subsections,...). The redifintion of
%% \@cftmaketoctitle is done since the tocloft package only prints the
%% title with a certain font, and like it as a chapter (without numbers). 
\RequirePackage{tocloft}
\setcounter{tocdepth}{1}
\renewcommand\contentsname{Table of Contents}
\renewcommand\listfigurename{List of Figures}
\renewcommand\listtablename{List of Tables}
%% Title defs
\renewcommand\@cftmaketoctitle{%
  \addpenalty\@secpenalty
  \@cftpagestyle
  {\interlinepenalty\@M
    \chapter*{\contentsname}
    \cftmarktoc
  }}
\renewcommand\@cftmakeloftitle{%
 \addpenalty\@secpenalty
 \@cftpagestyle
 {\interlinepenalty\@M
   \chapter*{\listfigurename}
   \cftmarklof
 }}
\renewcommand\@cftmakelottitle{%
 \addpenalty\@secpenalty
 \@cftpagestyle
 {\interlinepenalty\@M
   \chapter*{\listtablename}
   \cftmarklot
 }}
%% Get the right numbering
\tocloftpagestyle{fancy}
%% Now follows specifications for how to print the ToC.
%% chapter
\renewcommand\cftchapfont{\sffamily\bfseries}
\renewcommand\cftchappagefont{\sffamily\bfseries}
\renewcommand\cftchapleader{\hrulefill}
%% section
\renewcommand\cftsecfont{\sffamily\mdseries}
\renewcommand\cftsecpagefont{\sffamily\mdseries}
%% subsection
\renewcommand\cftsubsecfont{\sffamily\mdseries\relsize{-1}}
\renewcommand\cftsubsecpagefont{\sffamily\mdseries\relsize{-1}}
\renewcommand\cftsubsecleader{\hfill}
%% subsubsection
\renewcommand\cftsubsubsecfont{\sffamily\mdseries\relsize{-2}}
\renewcommand\cftsubsubsecpagefont{\sffamily\mdseries\relsize{-2}}
\renewcommand\cftsubsubsecleader{\hfill}
%% LoF
\renewcommand\cftfigfont{\sffamily\mdseries}
\renewcommand\cftfigpagefont{\sffamily\mdseries}
%% LoT
\renewcommand\cfttabfont{\sffamily\mdseries}
\renewcommand\cfttabpagefont{\sffamily\mdseries}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Bibliography. Set the title of bibliography in \bibname. Add to
%% \thebibliography environment that the header looks as before and
%% that it appears in the ToC. (To remove it from the ToC just
%% redefine \thebibliography exactly as below but without the last row.
\renewcommand\bibname{References}    % Title name for bibliography
\let\sh@thebibliography\thebibliography
\renewcommand\thebibliography[1]{%    % Put in ToC
  \sh@thebibliography{#1}
  \markright{\bibname}
  \addcontentsline{toc}{chapter}{\bibname}
}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Definitions for the Titlepage  (Some more TeX like definitions than before)
\def\Title#1{\gdef\sh@Title{#1}}
\def\Author#1{\gdef\sh@Author{#1}}
\def\Supervisor#1{\gdef\sh@Supervisor{#1}}
\def\Advisor#1{\gdef\sh@Supervisor{#1}}  % Backward comaptibility
\def\Examiner#1{\gdef\sh@Examiner{#1}}
\def\Date#1{\gdef\sh@Date{#1}}
\def\Year#1{\gdef\sh@Year{#1}}
\def\Company#1{\gdef\sh@Company{#1}}
%% Set default values
\Title{Title}
\Author{Author}
\Supervisor{Supervisor}
\Examiner{Examiner}
\Date{\today}
\Year{\the\year} %% this year
\Company{Department of Electrical and Information Technology\\Lund University}
%%
\newcommand\MakeTitlePage{% Print the title page
  \thispagestyle{empty}
  \strut\vfill
  \begin{center}
    \LARGE \sh@Title\\[2\baselineskip]
    \large \sh@Author\\[\baselineskip]
    \large \sh@Company\\[2\baselineskip]
    Supervisor: \sh@Supervisor\\[\baselineskip]
    Examiner: \sh@Examiner\\[\baselineskip]
    \sh@Date
  \end{center}
  \vfill\vfill\strut
  \newpage   %% copyright page
  \thispagestyle{empty}
  \strut\vfill
  \noindent\small
  \copyright\ \sh@Year\newline
  Printed in Sweden\newline
  Tryckeriet i E-huset, Lund
  %%
  \clearpage
  \setcounter{page}{1}
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\endinput
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
