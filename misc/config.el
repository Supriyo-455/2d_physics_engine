;; ============================================================
;; EMACS CONFIG - SUPRIYO PAUL
;; ============================================================


;; ============================================================
;; 1. UI & LAYOUT
;; ============================================================

;; Disable startup screen
(setq inhibit-startup-screen t)

;; Disable UI bars we don't need
(menu-bar-mode -1)
(when (fboundp 'tool-bar-mode)   (tool-bar-mode -1))
(when (fboundp 'scroll-bar-mode) (scroll-bar-mode -1))

;; Highlight the current line
(global-hl-line-mode 1)

;; On startup, delete any extra windows and split into 2 vertical panels
;; Uses split-window-right on Emacs 24+, falls back to split-window-horizontally on Emacs 22
(add-hook 'emacs-startup-hook
          (lambda ()
            (delete-other-windows)
            (if (fboundp 'split-window-right)
                (split-window-right)
              (split-window-horizontally))))


;; ============================================================
;; 2. FONT & COLORS
;; ============================================================

;; Consolas 14pt — clean monospace font, ships with Windows by default
(set-face-attribute 'default nil :family "Consolas" :height 140)

;; Base terminal green-on-black color scheme
(set-foreground-color "#A8C8A0") ;; soft sage green text
(set-background-color "#0D1210") ;; very dark green-black background
(set-cursor-color     "#39FF14") ;; bright neon green cursor

;; Enable multiline font-lock for better type/keyword detection
(setq font-lock-multiline t)
(setq-default font-lock-maximum-decoration t)

;; Force a full re-highlight pass after every save
;; Fixes inconsistent highlighting after edits in Emacs 22
(add-hook 'after-save-hook
          (lambda ()
            (when (member major-mode '(c-mode c++-mode))
              (font-lock-fontify-buffer))))

(custom-set-faces
 ;; --- Core UI ---
 '(default                      ((t (:foreground "#A8C8A0" :background "#0D1210"))))
 '(region                       ((t (:background "#1A3320"))))  ;; selection highlight
 '(highlight                    ((t (:background "#111A10"))))  ;; current line highlight
 '(fringe                       ((t (:background "#0D1210"))))  ;; left/right fringe
 '(minibuffer-prompt            ((t (:foreground "#39FF14" :weight bold))))

 ;; --- Syntax Highlighting ---
 '(font-lock-comment-face       ((t (:foreground "#3A5C3A" :slant italic)))) ;; muted green comments
 '(font-lock-string-face        ((t (:foreground "#5DBB63"))))               ;; mid green strings
 '(font-lock-keyword-face       ((t (:foreground "#39FF14" :weight bold))))  ;; neon green keywords
 '(font-lock-function-name-face ((t (:foreground "#74C365"))))               ;; mantis green functions
 '(font-lock-variable-name-face ((t (:foreground "#8FBC8F"))))               ;; dark sea green variables
 '(font-lock-type-face          ((t (:foreground "#00A550"))))               ;; deep green types
 '(font-lock-constant-face      ((t (:foreground "#00FF7F"))))               ;; spring green constants

 ;; --- Mode Line ---
 '(mode-line                    ((t (:foreground "#0D1210" :background "#39FF14")))) ;; active
 '(mode-line-inactive           ((t (:foreground "#3A5C3A" :background "#111A10")))) ;; inactive

 ;; --- Bracket Matching ---
 '(show-paren-match             ((t (:background "#1A5C1A" :foreground "#39FF14" :weight bold)))) ;; matching bracket
 '(show-paren-mismatch          ((t (:background "#5C1A1A" :foreground "#FF4444" :weight bold)))) ;; mismatched bracket

 ;; --- Compilation Buffer ---
 '(compilation-error            ((t (:foreground "#FF4444" :weight bold)))) ;; red errors
 '(compilation-warning          ((t (:foreground "#FFA500" :weight bold)))) ;; orange warnings
 '(compilation-info             ((t (:foreground "#39FF14" :weight bold)))) ;; green info/notes
 '(compilation-line-number      ((t (:foreground "#A8C8A0"))))              ;; soft line numbers
 '(compilation-column-number    ((t (:foreground "#A8C8A0"))))              ;; soft column numbers
 '(compilation-mode-line-exit   ((t (:foreground "#39FF14" :weight bold)))) ;; green on success
 '(compilation-mode-line-fail   ((t (:foreground "#FF4444" :weight bold)))) ;; red on failure
 '(compilation-mode-line-run    ((t (:foreground "#FFA500" :weight bold)))));; orange while running


;; ============================================================
;; 3. BRACKET MATCHING
;; ============================================================

;; Highlight matching bracket/brace when cursor is on one
;; Green = match found, Red = no match
(show-paren-mode 1)
(setq show-paren-delay 0) ;; no delay, instant highlight


;; ============================================================
;; 4. C/C++ STYLE
;; ============================================================

;; Use Linux style as base — closest to Allman with consistent brace indentation
(setq c-default-style "linux")
(setq-default c-basic-offset 4)     ;; 4 space indentation
(setq-default indent-tabs-mode nil) ;; spaces only, no tabs

;; Fix case label indentation inside switch blocks
(add-hook 'c-mode-common-hook
          (lambda ()
            (c-set-offset 'case-label 4)
            (c-set-offset 'statement-case-intro 4)))


;; ============================================================
;; 5. EDITING BEHAVIOR
;; ============================================================

(cua-mode 1)              ;; Standard Ctrl+C, Ctrl+V, Ctrl+X, Ctrl+Z
(setq truncate-lines nil) ;; Wrap long lines instead of truncating
(setq word-wrap t)        ;; Wrap at word boundaries not mid-word
(setq make-backup-files nil)  ;; No ~ backup files
(setq auto-save-default nil)  ;; No # autosave temp files

;; Enter auto-indents to the correct level
(global-set-key (kbd "RET") 'newline-and-indent)

;; Tab autocompletes from all open buffers using dabbrev
(global-set-key (kbd "TAB") 'dabbrev-expand)


;; ============================================================
;; 6. KEYBINDINGS
;; ============================================================

;; Save current file
(global-set-key (kbd "C-s") 'save-buffer)

;; Search — C-f starts search, keep pressing C-f to go to next match
(global-set-key (kbd "C-f")   'isearch-forward)
(global-set-key (kbd "C-S-f") 'isearch-backward)
(define-key isearch-mode-map (kbd "C-f")   'isearch-repeat-forward)
(define-key isearch-mode-map (kbd "C-S-f") 'isearch-repeat-backward)

;; Switch between left and right panel
(global-set-key (kbd "C-,") 'other-window)

;; Jump to next/previous error in compilation buffer
(global-set-key (kbd "<f4>")   'next-error)
(global-set-key (kbd "S-<f4>") 'previous-error)

;; Jump to function/type definition in other window (requires TAGS file)
;; TAGS file is auto-generated on every C/C++ save — see section 8
(global-set-key (kbd "M-.")
                (lambda () (interactive)
                  (find-tag-other-window (find-tag-default))))

;; Jump back to where you were before the tag jump
(global-set-key (kbd "M-,") 'pop-tag-mark)

;; Duplicate the current line below
(global-set-key (kbd "C-S-d")
                (lambda () (interactive)
                  (let ((line (buffer-substring (line-beginning-position)
                                                (line-end-position))))
                    (end-of-line)
                    (newline)
                    (insert line))))


;; ============================================================
;; 7. AUTO SAVE
;; ============================================================

;; Save all modified file buffers directly to disk
;; Triggers every 30 seconds of idle time or every 200 keystrokes
(defun full-auto-save ()
  (interactive)
  (save-excursion
    (dolist (buf (buffer-list))
      (set-buffer buf)
      (when (and (buffer-file-name) (buffer-modified-p))
        (basic-save-buffer)))))

(add-hook 'auto-save-hook 'full-auto-save)
(setq auto-save-timeout 30)  ;; seconds of idle before auto save
(setq auto-save-interval 200) ;; keystrokes before auto save


;; ============================================================
;; 8. AUTO FORMAT & TAGS
;; ============================================================

;; Indent the entire current buffer
(defun indent-buffer ()
  (interactive)
  (indent-region (point-min) (point-max)))

;; Only auto-format C/C++ files on save — skips bat, el, and other files
(defun indent-buffer-if-code ()
  (when (member major-mode '(c-mode c++-mode))
    (indent-buffer)))

(add-hook 'before-save-hook 'indent-buffer-if-code)

;; Regenerate TAGS file after every C/C++ save
;; Keeps M-. tag jumps and syntax highlighting consistent
(defun update-tags-after-save ()
  (when (member major-mode '(c-mode c++-mode))
    (let ((root (get-project-root)))
      (when root
        (shell-command
         (concat "cd /d " root "code && etags *.cpp *.h") nil nil)))))

(add-hook 'after-save-hook 'update-tags-after-save)


;; ============================================================
;; 9. CUSTOM HIGHLIGHTS
;; ============================================================

;; Highlight special comment markers in C/C++ files:
;;   TODO:      red   — something that needs to be done
;;   NOTE:      green — informational comment worth noticing
;;   IMPORTANT: pink  — critical, don't ignore this
(defun setup-custom-highlights ()
  (font-lock-add-keywords nil
                          '(("\\<\\(TODO\\):"      1 '(:foreground "#FF4444" :weight bold) t)
                            ("\\<\\(NOTE\\):"      1 '(:foreground "#39FF14" :weight bold) t)
                            ("\\<\\(IMPORTANT\\):" 1 '(:foreground "#FF69B4" :weight bold) t))))

(add-hook 'c-mode-common-hook 'setup-custom-highlights)


;; ============================================================
;; 10. BUILD SYSTEM
;; ============================================================

;; Cache the project root so it never changes when switching files
;; To reset manually: M-x eval-expression RET (setq my-project-root nil) RET
(setq my-project-root nil)

;; Walk up the directory tree until we find the folder containing misc/build.bat
;; Works correctly regardless of which source file is currently open
(defun get-project-root ()
  (unless my-project-root
    (let ((dir default-directory))
      (while (and dir
                  (not (file-exists-p (concat dir "misc/build.bat")))
                  (not (equal dir (file-name-directory (directory-file-name dir)))))
        (setq dir (file-name-directory (directory-file-name dir))))
      (setq my-project-root dir)))
  my-project-root)

;; F1 — Build the project
(global-set-key (kbd "<f1>") (lambda () (interactive)
                               (let ((default-directory (get-project-root)))
                                 (compile "misc\\build.bat"))))

;; F2 — Launch debugger (RAD Debugger)
(global-set-key (kbd "<f2>") (lambda () (interactive)
                               (let ((default-directory (get-project-root)))
                                 (compile "misc\\debug.bat"))))

;; F3 — Run the game
(global-set-key (kbd "<f3>") (lambda () (interactive)
                               (let ((default-directory (get-project-root)))
                                 (compile "misc\\run.bat"))))


;; ============================================================
;; UPGRADE BLOCK
;; Uncomment everything below after upgrading to Emacs 29
;; Requires clangd installed and on PATH
;; ============================================================

;; --- Package Manager Setup ---
;; (require 'package)
;; (setq package-user-dir (expand-file-name ".emacs_pkgs" default-directory))
;; (add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
;; (package-initialize)

;; --- Install LSP Mode if not present ---
;; (unless (package-installed-p 'lsp-mode)
;;   (package-refresh-contents)
;;   (package-install 'lsp-mode))

;; --- LSP Keybindings ---
;; (with-eval-after-load 'lsp-mode
;;   (define-key lsp-mode-map (kbd "M-.") 'lsp-find-definition)   ;; Jump to definition
;;   (define-key lsp-mode-map (kbd "M-,") 'xref-pop-marker-stack));; Jump back

;; --- Enable LSP for C/C++ buffers ---
;; (add-hook 'c++-mode-hook #'lsp-deferred)
;; (add-hook 'c-mode-hook   #'lsp-deferred)