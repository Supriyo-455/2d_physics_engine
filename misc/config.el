;; ============================================================
;; EMACS CONFIG - SUPRIYO PAUL
;; Emacs 29+ with LSP/Clangd
;; ============================================================


;; ============================================================
;; 1. PACKAGE MANAGER
;; ============================================================

(require 'package)
(setq package-user-dir (expand-file-name ".emacs_pkgs" default-directory))
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
(package-initialize)

;; Install LSP mode if not present
(unless (package-installed-p 'lsp-mode)
  (package-refresh-contents)
  (package-install 'lsp-mode))


;; ============================================================
;; 2. UI & LAYOUT
;; ============================================================

;; Disable startup screen and UI chrome
(setq inhibit-startup-screen t)
(menu-bar-mode   -1)
(tool-bar-mode   -1)
(scroll-bar-mode -1)

;; Highlight the current line
(global-hl-line-mode 1)

;; Show line numbers in code buffers
(add-hook 'c-mode-common-hook 'display-line-numbers-mode)

;; On startup split into 2 vertical panels
(add-hook 'emacs-startup-hook
          (lambda ()
            (delete-other-windows)
            (split-window-right)))


;; ============================================================
;; 3. FONT & COLORS
;; ============================================================

;; Consolas 14pt — clean monospace, ships with Windows
(set-face-attribute 'default nil :family "Consolas" :height 140)

;; Green on black color scheme
(set-foreground-color "#A8C8A0") ;; soft sage green text
(set-background-color "#0D1210") ;; very dark green-black
(set-cursor-color     "#39FF14") ;; bright neon green cursor

;; Simple consistent highlighting — avoids Emacs font-lock inconsistency bugs
(setq-default font-lock-maximum-decoration 1)

;; Force full re-highlight after every save
(add-hook 'after-save-hook
          (lambda ()
            (when (member major-mode '(c-mode c++-mode))
              (font-lock-fontify-buffer))))

(custom-set-faces
 ;; --- Core UI ---
 '(default                      ((t (:foreground "#A8C8A0" :background "#0D1210"))))
 '(region                       ((t (:background "#1A3320"))))  ;; selection
 '(highlight                    ((t (:background "#111A10"))))  ;; current line
 '(fringe                       ((t (:background "#0D1210"))))
 '(minibuffer-prompt            ((t (:foreground "#39FF14" :weight bold))))
 '(line-number                  ((t (:foreground "#2A4A2A" :background "#0D1210"))))
 '(line-number-current-line     ((t (:foreground "#39FF14" :background "#0D1210"))))

 ;; --- Syntax ---
 '(font-lock-comment-face       ((t (:foreground "#3A5C3A" :slant italic))))
 '(font-lock-string-face        ((t (:foreground "#5DBB63"))))
 '(font-lock-keyword-face       ((t (:foreground "#39FF14" :weight bold))))
 '(font-lock-function-name-face ((t (:foreground "#74C365"))))
 '(font-lock-preprocessor-face  ((t (:foreground "#39FF14" :weight bold))))
 '(font-lock-type-face          ((t (:foreground "#00A550"))))
 '(font-lock-constant-face      ((t (:foreground "#00FF7F"))))

 ;; --- Mode Line ---
 '(mode-line                    ((t (:foreground "#0D1210" :background "#39FF14"))))
 '(mode-line-inactive           ((t (:foreground "#3A5C3A" :background "#111A10"))))

 ;; --- Bracket Matching ---
 '(show-paren-match             ((t (:background "#1A5C1A" :foreground "#39FF14" :weight bold))))
 '(show-paren-mismatch          ((t (:background "#5C1A1A" :foreground "#FF4444" :weight bold))))

 ;; --- Compilation Buffer ---
 '(compilation-error            ((t (:foreground "#FF4444" :weight bold))))
 '(compilation-warning          ((t (:foreground "#FFA500" :weight bold))))
 '(compilation-info             ((t (:foreground "#39FF14" :weight bold))))
 '(compilation-line-number      ((t (:foreground "#A8C8A0"))))
 '(compilation-column-number    ((t (:foreground "#A8C8A0"))))
 '(compilation-mode-line-exit   ((t (:foreground "#39FF14" :weight bold))))
 '(compilation-mode-line-fail   ((t (:foreground "#FF4444" :weight bold))))
 '(compilation-mode-line-run    ((t (:foreground "#FFA500" :weight bold)))))


;; ============================================================
;; 4. BRACKET MATCHING
;; ============================================================

;; Highlight matching bracket instantly
;; Green = match found, Red = no match
(show-paren-mode 1)
(setq show-paren-delay 0)


;; ============================================================
;; 5. C/C++ STYLE
;; ============================================================

(setq c-default-style "linux")
(setq-default c-basic-offset 4)
(setq-default indent-tabs-mode nil)

;; Fix switch/case indentation
(add-hook 'c-mode-common-hook
          (lambda ()
            (c-set-offset 'case-label 4)
            (c-set-offset 'statement-case-intro 4)))


;; ============================================================
;; 6. EDITING BEHAVIOR
;; ============================================================

(cua-mode 1)                   ;; Ctrl+C, Ctrl+V, Ctrl+X, Ctrl+Z
(global-visual-line-mode 1)    ;; Wrap lines at word boundaries (Emacs 29)
(setq make-backup-files nil)   ;; No ~ backup files
(setq auto-save-default nil)   ;; No # autosave temp files

;; Enter auto-indents to correct level
(global-set-key (kbd "RET") 'newline-and-indent)

;; Tab autocompletes from open buffers
(global-set-key (kbd "TAB") 'dabbrev-expand)


;; ============================================================
;; 7. KEYBINDINGS
;; ============================================================

;; File
(global-set-key (kbd "C-s") 'save-buffer)

;; Search — C-f to start, keep pressing to go to next match
(global-set-key (kbd "C-f")   'isearch-forward)
(global-set-key (kbd "C-S-f") 'isearch-backward)
(define-key isearch-mode-map (kbd "C-f")   'isearch-repeat-forward)
(define-key isearch-mode-map (kbd "C-S-f") 'isearch-repeat-backward)

;; Panel switching
(global-set-key (kbd "C-,") 'other-window)

;; Error navigation
(global-set-key (kbd "<f4>")   'next-error)
(global-set-key (kbd "S-<f4>") 'previous-error)

;; Tag/definition jumping — opens in other window
(global-set-key (kbd "M-.")
                (lambda () (interactive)
                  (find-tag-other-window (find-tag-default))))
(global-set-key (kbd "M-,") 'pop-tag-mark)

;; Duplicate current line
(global-set-key (kbd "C-S-d")
                (lambda () (interactive)
                  (let ((line (buffer-substring (line-beginning-position)
                                                (line-end-position))))
                    (end-of-line)
                    (newline)
                    (insert line))))


;; ============================================================
;; 8. AUTO SAVE
;; ============================================================

;; Save all modified buffers every 30 seconds or 200 keystrokes
(defun full-auto-save ()
  (interactive)
  (save-excursion
    (dolist (buf (buffer-list))
      (set-buffer buf)
      (when (and (buffer-file-name) (buffer-modified-p))
        (basic-save-buffer)))))

(add-hook 'auto-save-hook 'full-auto-save)
(setq auto-save-timeout 30)
(setq auto-save-interval 200)


;; ============================================================
;; 9. AUTO FORMAT & TAGS
;; ============================================================

;; Indent entire buffer
(defun indent-buffer ()
  (interactive)
  (indent-region (point-min) (point-max)))

;; Auto-format only C/C++ files on save
(defun indent-buffer-if-code ()
  (when (member major-mode '(c-mode c++-mode))
    (indent-buffer)))

(add-hook 'before-save-hook 'indent-buffer-if-code)

;; Regenerate TAGS after every C/C++ save
;; Keeps M-. jumps and highlighting consistent
(defun update-tags-after-save ()
  (when (member major-mode '(c-mode c++-mode))
    (let ((root (get-project-root)))
      (when root
        (shell-command
         (concat "cd /d " root "code && etags *.cpp *.h") nil nil)))))

(add-hook 'after-save-hook 'update-tags-after-save)


;; ============================================================
;; 10. CUSTOM HIGHLIGHTS
;; ============================================================

;; Special comment markers:
;;   TODO:      red   — needs to be done
;;   NOTE:      green — informational
;;   IMPORTANT: pink  — critical
(defun setup-custom-highlights ()
  (font-lock-add-keywords nil
                          '(("\\<\\(TODO\\):"      1 '(:foreground "#FF4444" :weight bold) t)
                            ("\\<\\(NOTE\\):"      1 '(:foreground "#39FF14" :weight bold) t)
                            ("\\<\\(IMPORTANT\\):" 1 '(:foreground "#FF69B4" :weight bold) t))))

(add-hook 'c-mode-common-hook 'setup-custom-highlights)


;; ============================================================
;; 11. LSP / CLANGD
;; ============================================================

;; LSP keybindings — replaces etags jumps with proper clangd definitions
(with-eval-after-load 'lsp-mode
  (define-key lsp-mode-map (kbd "M-.") 'lsp-find-definition)
  (define-key lsp-mode-map (kbd "M-,") 'xref-pop-marker-stack))

;; Enable LSP for all C/C++ buffers
(add-hook 'c++-mode-hook #'lsp-deferred)
(add-hook 'c-mode-hook   #'lsp-deferred)


;; ============================================================
;; 12. BUILD SYSTEM
;; ============================================================

;; Cache project root — never changes when switching files
;; Reset: M-x eval-expression RET (setq my-project-root nil) RET
(setq my-project-root nil)

;; Walk up directory tree to find folder containing misc/build.bat
(defun get-project-root ()
  (unless my-project-root
    (let ((dir default-directory))
      (while (and dir
                  (not (file-exists-p (concat dir "misc/build.bat")))
                  (not (equal dir (file-name-directory (directory-file-name dir)))))
        (setq dir (file-name-directory (directory-file-name dir))))
      (setq my-project-root dir)))
  my-project-root)

;; F1 — Build
(global-set-key (kbd "<f1>") (lambda () (interactive)
                               (let ((default-directory (get-project-root)))
                                 (compile "misc\\build.bat"))))

;; F2 — Debug (RAD Debugger)
(global-set-key (kbd "<f2>") (lambda () (interactive)
                               (let ((default-directory (get-project-root)))
                                 (compile "misc\\debug.bat"))))

;; F3 — Run
(global-set-key (kbd "<f3>") (lambda () (interactive)
                               (let ((default-directory (get-project-root)))
                                 (compile "misc\\run.bat"))))
