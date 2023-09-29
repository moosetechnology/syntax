;;; Slightly modified Tue Aug 19 1986 by Philippe Deschamp (PhD)	;;;
;;; of INRIA to avoid a funny problem with keymaps (don't know which,	;;;
;;; as a matter of fact!).  Also to delete the temporary buffer when it	;;;
;;; is not useful any more.  Unhappily introduced "cur-" functions	;;;

; Elec-c is a set of routines to aid C program development.
; Written by Duane T. Williams
; 
; EXECUTION
;
; Once the package is loaded, local key bindings and local abbreviations
; are initialized within a buffer by executing the function elec-c-mode.
; The most convenient way to do this is by including in your .emacs_pro
; file the commands:
;	(autoload "elec-c-mode" "elec-c.ml")
;	(auto-execute "elec-c-mode" "*.c")
;
; DESCRIPTION
;
; The main functions provided by this package are:
; 
; 1. Expansion of C control constructs: if, else, while, do, switch,
; 	and for.  The user types the keyword immediately followed by
; 	a space, which causes the construct to be expanded, and, in
; 	the typical case, the user is placed in recusive edit mode to
; 	fill in () or {}.  When the filling in is complete, the user
;	types ^C to exit the recursive edit.  This should be done while
;	the cursor is still within the () or {}.
;
; 	NOTE:  the user is also placed in recursive edit mode to
; 	insert the constant expression following 'case' in a switch
; 	statement and should use ^C to move from the case constant
;	expression to the body of the case.
;
; 	For example, when the user types a space following "if" the 
; 	following appears in the buffer:
; 
; 		if () {
; 
; 		}
; 
; 	and the cursor is between the parentheses.  The user then 
; 	enters some boolean expression within the parens, followed by
; 	^C.  The ^C will exit the recursive edit and cause the cursor
; 	to move to the body of the if construct.  The user then enters
; 	additional statements, the last of which should be followed by ^C.
; 	The cursor will be left on a new line immediately below the
; 	right brace, "}".  If the user was on a blank line when he typed
; 	^C, that line will have been deleted.
; 
; 2. Various characters in C almost always come in pairs: {}, (), [].
; 	When the user types the first, he gets the second
; 	as well, with special formatting done for {}.  You can always
; 	quote (with ^Q) the left "paren" to avoid the expansion.
;
; 3. Semicolon and linefeed try to put the user where it is likely he
; 	wants to be.  The semicolon wants to go at the end of the line!
;
; 4. The slash and asterisk are bound to special functions that try to
; 	do the right thing as far as comments are concerned.  When you
;	type /* your will be put into commment mode, in which the control
;	constructs are not expanded, and when you subsequently type */
;	comment mode is terminated.
;
;	Comments are automatically justified.  You get BLOCKED comments
; 	when you type the &elec-c-border-char (the default is '-') after
; 	the opening "/*" of a comment.  To close a blocked comment, type
;	the &elec-c-border-char immediately after a subsequent '*',
; 	presumably at the left end of the block.
; 
; 5. Default settings of global variables:
; 	&elec-c-indent-width    4	; indentation of expressions
; 	&elec-c-right-margin	75	; fold comments here
; 	&elec-c-border-char	'-'	; char for block comments
; 	&elec-c-border-size	75	; border width
;	&elec-c-comment-prefix  "  "	; for non-block comments
;	&elec-c-split-declarations 0	; justification option
;
; 6. A C program justification function is bound to ESC-j.  It will
; 	re-indent a block (bounded by "{}" (at the left margin)).  It's
;	a bit slow; expect to wait.  You will be told when it's done.
;
; 7. #d and #i are defined as abbreviations for #define and #include,
; 	respectively.
;
; 8. There is a function called "comment" that allows you to enter
; 	comment mode manually (without typing "/*").
; 	This can be used in editing existing comments.  You have to be
; 	inside a comment when you toggle comment mode ON.  You can
; 	toggle comment mode OFF by either re-executing the comment
; 	function or, more simply, by typing ^C.
;
;    There is a function called "verbatim", which causes all special
;	elec-c processing to be turned off until the user types ^C.
;
; 9. "main" always goes at the left margin and expands as you expect it
; 	to.  Typing "argc" within the parameter list of "main"
; 	causes automatic insertion of argv and their declarations.
;
; 10. The quotation marks are bound to special functions that attempt to
;	prevent expansions when you are inside quotations.  If you find
;	that expressions are suddenly not expanding as they should, then
;	you have probably typed a quote mark without typing its closing
;	mate.
;
; HISTORY
; 03-Apr-83  Duane Williams (dtw) at Carnegie-Mellon University
;	Updated documentation and removed experimental functions in
;	preparation for installation on /usr/local.
;
; 17-May-82  Duane Williams (dtw) at Carnegie-Mellon University
;	Control constructs are now expanded via the new hooked
;	abbreviation function.
;
; 02-Feb-82  Duane Williams (dtw) at Carnegie-Mellon University
;	Created.

; PhD: I need that hack because neither (current-column) nor (current-indent)
; work on my station!
(defun
    (cur-column
	(save-excursion col here
	    (setq here (dot))
	    (setq col 1)
	    (beginning-of-line)
	    (while (!= (dot) here)
		   (if (= (following-char) '\^I')
		       (setq col (+ tab-size 1 (* tab-size (/ (- col 1) tab-size))))
		       (setq col (+ 1 col)))
		   (forward-character))
	    col))

    (cur-indent
	(save-excursion
	    (skip-line-indentation)
	    (cur-column)))
)

(progn
    (setq-default &elec-c-split-declarations 0) ; USER MAY CHANGE
    (setq-default &elec-c-indent-width 4)
    (setq-default &elec-c-right-margin 75)
    (setq-default &elec-c-border-char '-')
    (setq-default &elec-c-border-size 75)
    (setq-default &elec-c-comment-prefix "  ")
	
    (setq-default &elec-c-comment 0)		; PRIVATE VARIABLES
    (setq-default &elec-c-box-comment 0)
    (setq-default inside-double-quote 0)
    (setq-default inside-single-quote 0)
	
    (declare-buffer-specific	; allow these variables to have
	&elec-c-indent-width	; different values in each
	&elec-c-comment		; elec-c buffer
	&elec-c-box-comment
	&elec-c-right-margin
	&elec-c-border-char
	&elec-c-border-size
	&elec-c-comment-prefix
 	inside-double-quote
	inside-single-quote
    )

    (defun
	(current-position (+ (dot) 0))
	
	; if inside a comment, returns the column of the star
	; that initiates the comment; otherwise returns zero.
	;
	(inside-c-comment position star-position
	    (setq &elec-c-box-comment 0)
	    (setq position (current-position))
	    (save-excursion
		(if (error-occurred (search-reverse "/*"))
		    0
		    (progn
			(setq star-position (+ 1 (cur-column)))
			(if (looking-at 
				(concat "/\\*" 
				    (char-to-string &elec-c-border-char)))
			    (setq &elec-c-box-comment 1))
			(if (| (error-occurred (search-forward "*/"))
				(< position (current-position)))
			    star-position 0)
		    )
		)
	    )
	)
	
	(backward-brace level
	    (if (error-occurred (search-reverse "}"))
		(error-message "Can't find end of block!"))
	    (setq level 1)
	    (while (> level 0)
		(re-search-reverse "[{}]")
		(if (= (following-char) '{')
		    (setq level (- level 1))
		    (setq level (+ level 1)))
	    )
	)
	
	(forward-brace level
	    (if (error-occurred (search-forward "{"))
		(error-message "Can't find beginning of block!"))
	    (setq level 1)
	    (while (> level 0)
		(re-search-forward "[{}]")
		(if (= (preceding-char) '{')
		    (setq level (+ level 1))
		    (setq level (- level 1)))
	    )
	)
	
	(expand-abbrev space-position	; cause abbrev expansion without
	    (setq space-position (dot)) ; leaving extra chars in buffer
	    (insert-character '#')
	    (backward-character)
	    (insert-character ' ')
	    (save-window-excursion
		(goto-character space-position)
		(if (= (following-char) ' ')
		    (provide-prefix-argument 2
			(delete-next-character))
		    (delete-next-character)
		)
	    )
	)
	
	(pause-or-exit			; avoid accidentally exiting
	    (if (< (recursion-depth) 1)	; emacs
		(pause-emacs)
		(exit-emacs)
	    )
	)
	
	(pop-recursion			; leave recursive edit
	    (expand-abbrev)
	    (pause-or-exit)
	)
	
	(skip-white-space		; skip over spaces and tabs
	    (while (& (! (eolp)) 
		       (| (= (following-char) ' ') 
			   (= (following-char) '	')))
		(forward-character))
	)
	
	(this-line-is-blank		; true if only white space
	    (save-excursion		; on current line
		(beginning-of-line)
		(skip-white-space)
		(if (eolp) 1 0))
	)
	
	(kill-current-line		; delete current line
	    (beginning-of-line)
	    (if (eolp)
		(kill-to-end-of-line)
		(provide-prefix-argument 2 (kill-to-end-of-line)))
	)

	(beginning-of-block  mymark	; jump to head of {} block
	    (insert-string "}")
	    (setq mymark (dot))
	    (if (error-occurred (backward-brace))
		(progn
		    (goto-character mymark)
		    (delete-previous-character))
		(save-excursion
		    (goto-character mymark)
		    (delete-previous-character))
	    )
	)
	
	(end-of-block			; jump to end of {} block
	    (beginning-of-block)
	    (forward-brace)
	)
	
	
	(preceding-word			; return preceding word
	    (backward-word)
	    (set-mark)
	    (forward-word)
	    (region-to-string)
	)
    )

    (defun
	(elec-left-brace next-position old-mode	; insert brace pair
	    (setq old-mode mode-string)
	    (if (!(eolp)) (end-of-line))	; else a mess
	    (if (& (!(bolp))			; beautify
		    (!= (preceding-char) ' ')	; space
		    (!= (preceding-char) '\t'))	; tab
		(insert-character ' '))
	    (setq mode-string "block body")
	    (insert-string "{}")		; insert braces
	    (backward-character)		; and format
	    (newline-and-indent)
	    (forward-character)			; find next position
	    (if (eobp)				; avoid end of buffer
		(progn
		    (newline)
		    (backward-character))
	    )
	    (setq next-position (dot))
	    (previous-line)			; position for edit
	    (elec-indent)
	    (recursive-edit)
	    (if (this-line-is-blank)
		(kill-current-line)
	    )
	    (goto-character next-position)
	    (newline-and-indent)
	    (setq mode-string old-mode)
	)
	
	(elec-left-paren next-position		; insert paren pair
	    (insert-string "()")
	    (backward-character)
	)
	
	(elec-left-bracket next-position	; insert bracket pair
	    (insert-string "[]")
	    (backward-character)
	)
	
	(elec-right-paren line next-position
	    (insert-character (last-key-struck))
	    (save-excursion
		(backward-paren)
		(if (dot-is-visible)
		    (progn
			(setq line "")
			(sit-for 10))
		    (progn
			(beginning-of-line)
			(set-mark)
			(end-of-line)
			(setq line (region-to-string)))
		)
	    )
	    (message line)
	)
	
	(elec-tab newcol
	    (setq newcol (+ (cur-column) &elec-c-indent-width))
	    (insert-character '.')
	    (backward-character)
	    (delete-white-space)
	    (delete-next-character)
	    (to-col newcol)
	)
	
	(elec-function-format old-dot old-size
	    (setq old-dot (+ (dot) 0))
	    (setq old-size (buffer-size))
	    (save-excursion
		(previous-line)
		(re-search-forward "^}")
		(set-mark)
		(backward-brace)
		(beginning-of-line)
		(exchange-dot-and-mark)
		(end-of-line)
		(forward-character)
		(filter-region 
		    (concat 
			"indent " 
			"-st "
			"-i" &elec-c-indent-width
			(if  &elec-c-split-declarations
			    " -bc "
			    " -nbc "))
		)
	    )
	    (goto-character (/ (* (buffer-size) old-dot) old-size))
	    (message "Done!")
	)

	(elec-indent
	    (end-of-line)
	    (newline-and-indent)
	    (provide-prefix-argument &elec-c-indent-width
		(insert-character ' '))
	)
	
	(elec-linefeed
	    (if (eolp)
		(progn
		    (newline-and-indent))
		(progn
		    (elec-indent))
	    )
	)
	
	(elec-semicolon
	    (if (!(eolp)) (end-of-line))
	    (delete-white-space)
	    (insert-character ';')
	    (newline-and-indent)
	)
	
	(elec-main next-position old-mode
	    (setq old-mode mode-string)
	    (beginning-of-line)			; put main at margin
	    (delete-white-space)
	    (end-of-line)
	    (insert-string " (){}")		; expand and format
	    (provide-prefix-argument 2 (backward-character))
	    (newline)
	    (forward-character)
	    (newline)
	    (forward-character)
	    (newline)
	    (search-reverse ")")		; position for params
	    (save-window-excursion 
		(search-forward "{")		; find next position
		(setq next-position (dot))
	    )
	    (setq mode-string "main parameters")
	    (use-abbrev-table "elec-c-main-abbrevs")
	    (local-bind-to-key "pop-recursion" "\^C")
	    (recursive-edit)			; edit params
	    (use-abbrev-table "elec-c-abbrevs")
	    (local-bind-to-key "pause-or-exit" "\^C")
	    (goto-character next-position)
	    (newline)				; open body
	    (elec-indent)
	    (save-window-excursion 		; find next position
		(if (error-occurred (search-reverse "main"))
		    (search-reverse "main"))
		(forward-brace)
		(setq next-position (dot))
	    )
	    (setq mode-string "main body")
	    (recursive-edit)			; edit body
	    (goto-character next-position)
	    (newline)
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-argc 				; auto declare argc/argv
	    (search-forward ")")
	    (backward-character)
	    (insert-string ",argv")
	    (end-of-line)
	    (newline)
	    (insert-string "int argc;")
	    (newline)
	    (insert-string "char *argv[];")
	    0
	)
	
	(elec-if next-position old-mode		; insert if construct
	    (setq old-mode mode-string)
	    (insert-string " () {}")
	    (backward-character)
	    (newline-and-indent)
	    (previous-line)
	    (search-forward "(")
	    (save-window-excursion 
		(elec-indent)
		(setq next-position (dot))
	    )
	    (setq mode-string "if condition")
	    (recursive-edit)
	    (goto-character next-position)
	    (save-window-excursion 
		(search-forward "}")
		(setq next-position (dot))
	    )
	    (setq mode-string "if body")
	    (recursive-edit)
	    (if (this-line-is-blank)
		(kill-current-line)
	    )
	    (goto-character next-position)
	    (newline-and-indent)
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-else next-position old-mode	; insert else construct
	    (setq old-mode mode-string)
	    (insert-string " {}")
	    (backward-character)
	    (newline-and-indent)
	    (previous-line)
	    (elec-indent)
	    (save-window-excursion 
		(search-forward "}")
		(setq next-position (dot))
	    )
	    (setq mode-string "else body")
	    (recursive-edit)
	    (if (this-line-is-blank)
		(kill-current-line)
	    )
	    (goto-character next-position)
	    (newline-and-indent)
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-for next-position old-mode	; insert for construct
	    (setq old-mode mode-string)
	    (insert-string " (;;) {}")
	    (backward-character)
	    (newline-and-indent)
	    (previous-line)
	    (search-forward "(")
	    (save-window-excursion 
		(elec-indent)
		(setq next-position (dot))
	    )
	    (setq mode-string "for control")
	    (recursive-edit)
	    (goto-character next-position)
	    (save-window-excursion 
		(search-forward "}")
		(setq next-position (dot))
	    )
	    (setq mode-string "for body")
	    (recursive-edit)
	    (if (this-line-is-blank)
		(kill-current-line)
	    )
	    (goto-character next-position)
	    (newline-and-indent)
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-while next-position old-mode	; insert while construct
	    (setq old-mode mode-string)
	    (insert-string " () {}")
	    (backward-character)
	    (newline-and-indent)
	    (previous-line)
	    (search-forward "(")
	    (save-window-excursion 
		(elec-indent)
		(setq next-position (dot))
	    )
	    (setq mode-string "while condition")
	    (recursive-edit)
	    (goto-character next-position)
	    (save-window-excursion 
		(search-forward "}")
		(setq next-position (dot))
	    )
	    (setq mode-string "while body")
	    (recursive-edit)
	    (if (this-line-is-blank)
		(kill-current-line)
	    )
	    (goto-character next-position)
	    (newline-and-indent)
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-do next-position old-mode		; insert do construct
	    (setq old-mode mode-string)
	    (insert-string " {} while ();")
	    (search-reverse "}")
	    (newline-and-indent)
	    (previous-line)
	    (elec-indent)
	    (save-window-excursion 
		(search-forward "(")
		(setq next-position (dot))
	    )
	    (setq mode-string "do body")
	    (recursive-edit)
	    (if (this-line-is-blank)
		(kill-current-line)
	    )
	    (goto-character next-position)
	    (save-window-excursion 
		(search-forward ";")
		(setq next-position (dot))
	    )
	    (setq mode-string "do condition")
	    (recursive-edit)
	    (goto-character next-position)
	    (newline-and-indent)
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-switch next-position old-mode	; insert switch construct
	    (setq old-mode mode-string)
	    (insert-string " () {}")
	    (backward-character)
	    (newline-and-indent)
	    (previous-line)
	    (search-forward "(")
	    (save-window-excursion 
		(elec-indent)
		(setq next-position (dot))
	    )
	    (setq mode-string "switch condition")
	    (recursive-edit)
	    (goto-character next-position)
	    (save-window-excursion 
		(search-forward "}")
		(setq next-position (dot))
	    )
	    (elec-case-loop)
	    (goto-character next-position)
	    (newline-and-indent)
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-case next-position old-mode	; single case statment
	    (setq old-mode mode-string)
	    (insert-string " :break;")		; expand case statement
	    (search-reverse "b")
	    (newline-and-indent)
	    (provide-prefix-argument &elec-c-indent-width
		(insert-character ' '))
	    (search-reverse ":")		; goto case condition
	    (save-window-excursion 		; prepare next position
		(elec-indent)
		(setq next-position (dot))
	    )
	    (setq mode-string "case condition")
	    (recursive-edit)			; edit case condition
	    (goto-character next-position)
	    (save-window-excursion 
		(search-forward ";")
		(setq next-position (dot))
	    )
	    (setq mode-string "case body")
	    (recursive-edit)
	    (if (this-line-is-blank)
		(kill-current-line)
	    )
	    (goto-character next-position)
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-case-loop case-col next-position old-mode
	    (setq old-mode mode-string)
	    (setq case-col (cur-indent))	; save case column
	    (insert-string "case :break;")	; expand case statement
	    (search-reverse "b")
	    (newline-and-indent)
	    (provide-prefix-argument &elec-c-indent-width
		(insert-character ' '))
	    (search-reverse ":")		; goto case condition
	    (save-window-excursion 		; prepare next position
		(elec-indent)
		(setq next-position (dot))
	    )
	    (setq mode-string "case condition")
	    (recursive-edit)			; edit case condition
	    (while (!= (preceding-char) ' ')
		(goto-character next-position)	; goto case body
		(save-window-excursion 		; prepare next position
		    (search-forward ";")
		    (newline)
		    (to-col case-col)
		    (setq next-position (dot))
		)
		(setq mode-string "case body")
		(recursive-edit)		; edit body of case
		(if (this-line-is-blank)
		    (kill-current-line)
		)
		(goto-character next-position)	; find end of case
		(insert-string "case :break;")	; setup next case
		(search-reverse "b")
		(newline-and-indent)
		(provide-prefix-argument &elec-c-indent-width
		    (insert-character ' '))
		(search-reverse ":")		; goto case condition
		(save-window-excursion 		; prepare next position
		    (elec-indent)
		    (setq next-position (dot))
		)
		(setq mode-string "case condition")
		(recursive-edit)		; edit case condition
	    )
	    (delete-previous-word)		; setup for default
	    (insert-string "default")
	    (next-line)
	    (setq mode-string "default")
	    (recursive-edit)
	    (if (this-line-is-blank)
		(kill-current-line)
	    )
	    (setq mode-string old-mode)
	    0
	)
	
	(elec-default next-position old-mode	; case default
	    (setq old-mode mode-string)
	    (insert-string ":break;")
	    (search-reverse "b")
	    (newline-and-indent)
	    (provide-prefix-argument &elec-c-indent-width
		(insert-character ' '))
	    (previous-line)
	    (elec-indent)
	    (save-window-excursion 
		(search-forward ";")
		(setq next-position (dot))
	    )
	    (recursive-edit)
	    (goto-character next-position)
	    (setq mode-string old-mode)
	    0
	)
	
	(setup-for-code
	    (setq &elec-c-comment 0)
	    (setq left-margin 0)
	    (setq right-margin 0)
	    (setq prefix-string "")
	    (use-local-map "elec-c-keymap")
	    (setq abbrev-mode 1)
	)
	
	(elec-slash				; watch for close comment
	    (if (= (preceding-char) '*')
		(progn
		    (insert-character '/')
		    (pause-or-exit)
		)
		(insert-character '/')
	    )
	)

	(verbatim old-mode next-position
	    (setq old-mode mode-string)
	    (setq next-position (dot))
	    (setq left-margin 0)
	    (setq right-margin 0)
;	    (setq prefix-string "")
	    (setq abbrev-mode 0)
	    (use-local-map "elec-c-verbatim-keymap")
	    (setq mode-string "verbatim")
	    (message "Type ^C to exit verbatim mode")
	    (recursive-edit)
	    (goto-character next-position)
	    (setq mode-string old-mode)
	    (if &elec-c-comment
		(setup-for-comment)
		(setup-for-code)
	    )
	)

	(setup-for-comment
	    (if (!= (preceding-char) '/')
		(if (=  0 (setq left-margin (inside-c-comment)))
		    (setq left-margin (cur-column))
		)
		(setq left-margin (cur-column))
	    )
	    (setq right-margin &elec-c-right-margin)
	    (if &elec-c-box-comment		; set by inside-c-comment
		(setq prefix-string "* ")
		(setq prefix-string &elec-c-comment-prefix)
	    )
	    (use-local-map "elec-c-comment-keymap")
	    (setq abbrev-mode 0)
	    (setq &elec-c-comment 1)
	)
	    
	(elec-star old-mode			; watch for open comment
	    (if (= (preceding-char) '/')
		(progn
		    (insert-character '*')
		    (comment)
		)
		(insert-character '*')
	    )
	)
	    
	(elec-border old-prefix			; create comment border
	    (if (& &elec-c-comment 
		    (= (preceding-char) '*'))
		(progn 
		    (setq right-margin 10000)
		    (provide-prefix-argument &elec-c-border-size
			(insert-character &elec-c-border-char))
		    (newline-and-indent)
		    (setq right-margin &elec-c-right-margin)
		    (insert-character '.')
		    (backward-character)
		    (previous-line)
		    (if (= (following-char) '/')
			(progn
			    (next-line)
			    (delete-next-character)
			    (insert-string " * ")
			    (setq old-prefix prefix-string)
			    (setq prefix-string "* "))
			(= (following-char) '*')
			(progn
			    (setq prefix-string old-prefix)
			    (next-line)
			    (delete-next-character)
			    (insert-string "*/")
			    (newline)
			    (pause-or-exit))
		    )
		)
		(progn
		    (insert-character &elec-c-border-char))
	    )
	)
	    
	(comment old-mode		; toggle comment mode manually
	    (if &elec-c-comment
		(pause-or-exit)
		(progn 
		    (setq old-mode mode-string)
		    (setq mode-string "comment")
		    (setup-for-comment)
		    (recursive-edit)
		    (setup-for-code)
		    (setq mode-string old-mode)
		    (novalue)
		)
	    )
	)
	    
	(elec-doublequote	; guard quotes against abbrev expansion
	    (insert-string """")
	    (if (! inside-single-quote)
		(if inside-double-quote
		    (progn
			(setq inside-double-quote 0)
			(setq abbrev-mode 1))
		    (progn
			(setq inside-double-quote 1)
			(setq abbrev-mode 0))
		)
	    )
	)
	    
	(elec-singlequote	; guard quotes against abbrev expansion
	    (insert-string "'")
	    (if (! inside-double-quote)
		(if inside-single-quote
		    (progn
			(setq inside-single-quote 0)
			(setq abbrev-mode 1))
		    (progn
			(setq inside-single-quote 1)
			(setq abbrev-mode 0))
		)
	    )
	)		
	    
	(elec-c-mode				; initialize elec-c mode
	    (use-local-map "elec-c-keymap")
	    (use-abbrev-table "elec-c-abbrevs")
	    (setq mode-string "elec-c")
	    (use-syntax-table "elec-c")
	)
    )

    (save-excursion
	(temp-use-buffer "elec-c buffer")
	(setq needs-checkpointing 0)
	
;(if (error-occurred (use-local-map "elec-c-keymap"))
	    (progn
		(define-keymap "elec-c-keymap")
		(use-local-map "elec-c-keymap"))
;)
	(local-bind-to-key "backward-brace" "\e{")
	(local-bind-to-key "forward-brace" "\e}")
	(local-bind-to-key "elec-function-format" "\ej")
	(local-bind-to-key "elec-left-brace" '{')
    	(local-bind-to-key "elec-left-bracket" '[')
	(local-bind-to-key "elec-left-paren" '(')
	(local-bind-to-key "elec-linefeed" "\n")
	(local-bind-to-key "elec-doublequote" """")
	(local-bind-to-key "elec-singlequote" "'")
	(local-bind-to-key "elec-right-paren" '}')
	(local-bind-to-key "elec-right-paren" ')')
	(local-bind-to-key "elec-right-paren" ']')
	(local-bind-to-key "elec-semicolon" ';')
	(local-bind-to-key "elec-star" '*')
	(local-bind-to-key "elec-tab" "\t")
	(local-bind-to-key "pause-or-exit" "\^C")
	
;(if (error-occurred (use-local-map "elec-c-comment-keymap"))
	    (progn
		(define-keymap "elec-c-comment-keymap")
		(use-local-map "elec-c-comment-keymap"))
;)
	(local-bind-to-key "elec-slash" '/')
	(local-bind-to-key "elec-border" &elec-c-border-char)
	
;(if (error-occurred (use-local-map "elec-c-verbatim-keymap"))
	    (progn
		(define-keymap "elec-c-verbatim-keymap")
		(use-local-map "elec-c-verbatim-keymap"))
;)
	
	(use-abbrev-table "elec-c-abbrevs")
	(define-local-abbrev "#d" "#define")
	(define-local-abbrev "#i" "#include")
	(define-hooked-local-abbrev "if" "if" "elec-if")
	(define-hooked-local-abbrev "else" "else" "elec-else")
	(define-hooked-local-abbrev "for" "for" "elec-for")
	(define-hooked-local-abbrev "while" "while" "elec-while")
	(define-hooked-local-abbrev "do" "do" "elec-do")
	(define-hooked-local-abbrev "switch" "switch" "elec-switch")
	(define-hooked-local-abbrev "case" "case" "elec-case")
	(define-hooked-local-abbrev "default" "default" "elec-default")
	(define-hooked-local-abbrev "main" "main" "elec-main")

	(use-abbrev-table "elec-c-main-abbrevs")
	(define-hooked-local-abbrev "argc" "argc" "elec-argc")

	(use-syntax-table "elec-c")
	(modify-syntax-entry "w    #_")
	(modify-syntax-entry "(}   {")
	(modify-syntax-entry "(]   [")
	(modify-syntax-entry "()   (")
	(modify-syntax-entry "){   }")
	(modify-syntax-entry ")[   ]")
	(modify-syntax-entry ")(   )")
	(modify-syntax-entry "     """) ; else forward/backward paren fails
	(modify-syntax-entry "     '") ; else forward/backward paren fails
	(modify-syntax-entry "  {}*/")
	(modify-syntax-entry "\\    \\")
	(delete-buffer "elec-c buffer")
	(novalue)
    )
)
